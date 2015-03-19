#include <assert.h>
#include <iostream>
#include <list>
#include <string>
#include <mpi.h>

#include "MW_Master.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MPIMessage.hpp"

MW_Master::MW_Master(const int myid, const int sz, std::list<Work *> *work)
{
  id = myid;
  world_size = sz;

  workToDo = work;
  results = new std::list<Result *>();
  //std::cout << "Total work in master is " << workToDo->size() << std::endl;

  // Prepopulate the list of workers
  workers = new std::list<int>();
  for (int i=0; i<world_size; i++) {
    if (i != id) {
      workers->push_back(i);
    }
  }
}

void MW_Master::send_done()
{
  //std::cout << "MASTER SENDING DONE MESSAGES\n";
  for (int process_id=0; process_id<world_size; process_id++) {
    if (process_id != id) {
      //std::cout << "MASTER SENDING DONE to PROCESS" << process_id << " \n";
      MPI::COMM_WORLD.Send(
        0,
        0,
        MPI::CHAR,
        process_id,
        DONE_TAG
      );
    }
  }
}

void MW_Master::send(int worker_id)
{
  // std::cout << "P:" << this->id << " sending work to process " << worker_id << std::endl;

  Work *work = workToDo->front();
  workToDo->pop_front();

  std::string *work_string = work->serialize();
  int count = (int) work_string->length();

  // std::cout << "P:" << id << " sending work with " << count <<
  //   " total MPI::CHARs -- " << *work_string << std::endl;

  MPI::COMM_WORLD.Send(
    (void *) work_string->data(),
    count,
    MPI::CHAR,
    worker_id,
    WORK_TAG
  );

  // std::cout << "P:" << id << " finished send to P" << worker_id << ". " <<
  //   workToDo->size() << " work items remaining" << std::endl;

  delete work;
  delete work_string;
}

void MW_Master::workLoop()
{
  int worker_id;
  while (1) {
    if (hasWorkersHasWork()) {

      // std::cout << "MASTER IS SENDING\n";
      worker_id = nextWorker();
      send(worker_id);

    } else if (noWorkersHasWork() || noWorkersNoWork()) {

      // std::cout << "MASTER IS WAITING FOR A RESULT\n";
      receive();

    } else if (hasWorkersNoWork()) {
      if (hasAllWorkers()) {
        // std::cout << "MASTER IS DONE\n";
        send_done();

        break;
      } else {
        receive();
      }
    } else {
      std::cout << "WTF happened here\n";
      assert(0);
    }
  }
}


enum MwTag MW_Master::receive()
{
  // std::cout << "P:" << id << " master waiting to receive" << std::endl;

  MPI::Status status;
  // std::string& message = new std::string(1000, 0);
  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    MPI::ANY_SOURCE,
    WORK_TAG,
    status
  );

  int worker_id = status.Get_source();
  int message_tag = status.Get_source();
  int count = status.Get_count(MPI::CHAR);
  if (count != 0) {
    std::string serializedObject = std::string(message, count);
    // std::cout << "P:" << id << " Received from process " << worker_id <<
    //   " message of length "<< count << " \"" << serializedObject << "\"\n";

    // std::cout << "App is: " << app << std::endl;
    // Result *result = app->resultDeserializer(serializedObject);
    MPIMessage *message = new MPIMessage(serializedObject);
    // std::cout << "P:" << id << " message (result) is " << message->to_string() << std::endl;
    Result *result = message->deserializeResult();
    delete message;

    // std::cout << "P:" << id << " received results (" << result << ") from process " << worker_id << ". " << std::endl;
    assert(result != NULL);
    results->push_back(result);
    // std::cout << "results size is " << results->size() << std::endl;
  }

  free(message);
  // Reinclude this working in the queue
  workers->push_back(worker_id);
  static_cast<MwTag>(message_tag);
}


MW_Master::~MW_Master()
{
  // workToDo should be empty, so deleting its elements is bonus caution
  for ( auto iter = workToDo->begin();
      iter != workToDo->end();
      iter++)
  {
    Work *work = *iter;
    delete work;
  }

  delete workToDo;


  for ( auto iter = results->begin();
      iter != results->end();
      iter++)
  {
    Result *result = *iter;
    delete result;
  }

  delete results;
}


bool MW_Master::hasWorkersHasWork()
{
  return !workers->empty() && !workToDo->empty();
}

bool MW_Master::hasWorkersNoWork()
{
  return !workers->empty() && workToDo->empty();
}

bool MW_Master::noWorkersHasWork()
{
  return workers->empty() && !workToDo->empty();
}

bool MW_Master::noWorkersNoWork()
{
  return workers->empty() && workToDo->empty();
}

bool MW_Master::hasAllWorkers()
{
  return workers->size() == world_size - 1;  // exclude self (master)
}


int MW_Master::nextWorker()
{
  int worker_id = workers->front();
  assert(worker_id != 0);
  workers->pop_front();

  return worker_id;
}
