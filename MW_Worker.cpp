#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"

// MW_Worker::MW_Worker(const int myid, const int m_id, MW_API *mwapp)
MW_Worker::MW_Worker(const int myid, const int m_id)
{
  id = myid;
  master_id = m_id;

  // Blanks lists for work and results
  workToDo = new std::list<Work *>();
  results = new std::list<std::shared_ptr<Result>>();
}

enum MwTag MW_Worker::receive()
{
  // std::cout << "P:" << id << " waiting to receive work from master." << std::endl;

  MPI::Status status;
  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  // std::string message(1000, 0);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    master_id,
    MPI::ANY_TAG,
    // MW_Worker::WORK_TAG,
    status
  );

  int message_tag = status.Get_tag();

  if (message_tag == WORK_TAG) {
    std::string serializedObject = std::string(message, status.Get_count(MPI::CHAR));
    // std::cout << "P:" << id << " Received from master P" << master_id << " message \"" << serializedObject << "\"\n";

    MPIMessage *message = new MPIMessage(serializedObject);
    // std::cout << "P:" << id << " message (work) is " << message->to_string() << std::endl;
    Work *work = message->deserializeWork();
    delete message;
    assert(work != NULL);
    // std::cout << "P:" << id << " Recreated work object (" << work << ") \"" << *work->serialize() << "\"\n" ;

    workToDo->push_back(work);
    // std::cout << "P:" << id << " WorkToDo size is " << workToDo->size() << std::endl;
  } else {
    // Do nothing
  }

  free(message);
  return static_cast<MwTag>(message_tag);
}

void MW_Worker::send()
{
  // std::cout << "P:" << id << " Beginning send to master P" << master_id << std::endl;
  std::shared_ptr<Result> result = results->front();
  assert(result);
  results->pop_front();

  // std::cout << " Popped result (" << result << ") from list\n";

  std::string *result_string = result->serialize();
  int count = (int) result_string->length();

  // std::cout << "P:" << id << " sending result with " << count <<
  //   " total MPI::CHARs -- " << result_string << std::endl;

  assert(count < MAX_MESSAGE_SIZE);
  MPI::COMM_WORLD.Send(
    (void *) result_string->data(),
    count,
    MPI::CHAR,
    master_id,
    WORK_TAG
  );

  // std::cout << "P:" << id << " finished send to master P" << master_id << ". " <<
  //   results->size() << " results remaining" << std::endl;

  // delete result;
  delete result_string;
}

MW_Worker::~MW_Worker()
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


  // for ( auto iter = results->begin();
  //     iter != results->end();
  //     iter++)
  // {
  //   Result *result = *iter;
  //   delete result;
  // }

  delete results;
}

void MW_Worker::doWork()
{
  // std::cout << "P:" << worker->id << " Grabbing some workToDo\n";
  Work *work = workToDo->front();
  assert(work != NULL);
  // std::cout << "P:" << worker->id << " Work object is (" << work << ") \"" << work->serialize() << "\"\n";
  workToDo->pop_front();

  // std::cout << "P:" << worker->id << " Computing results.\n";

  MW_API_STATUS_CODE status;
  do
  {
    // std::cout << "Entered Loop" << std:: endl;
    status = work->compute(preemptionSemaphore);
    
  } while(status == Preempted);

  // std::cout << "Exited Loop" << std:: endl;

  std::shared_ptr<Result> new_result = work->result();
  // std::cout << new_result << std:: endl;
  // std::cout << "generated results" << std:: endl;
  
  assert(new_result != NULL);
  // std::cout << "P:" << worker->id << " Result object is (" << one_result << ") \"" << one_result->serialize() << "\"\n";

  results->push_back(new_result);
}

void MW_Worker::worker_loop()
{
  // Result *result;
  enum MwTag message_tag;
  while (1) {

    message_tag = receive();

    if (message_tag == WORK_TAG) {

      doWork();
      send();

    } else if (message_tag == DONE_TAG) {
      // std::cout << "P:" << proc->id << " IS DONE\n";
      break;
    } else {
      std::cout << "WTF happened here\n";
      assert(0);
    }
  }
}
