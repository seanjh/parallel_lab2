#include "MW_Master.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <iostream>
#include <list>
#include <string>
#include <mpi.h>

MW_Master::MW_Master(const int myid, const int sz, MW_API *app)
{
  id = myid;
  world_size = sz;
  app = app;

  workToDo = app->work();
  results = new std::list<Result *>();
  std::cout << "Total work in master is " << workToDo->size() << std::endl;

  // Prepopulate the list of workers
  workers = new std::list<int>();
  for (int i=0; i<world_size; i++) {
    if (i != id) {
      workers->push_back(i);
    }
  }
}

void MW_Master::send_one(int worker_id)
{
  std::cout << "P:" << this->id << " sending work to process " << worker_id << std::endl;

  Work *work = workToDo->front();
  workToDo->pop_front();

  std::string *work_string = work->serialize();
  int count = (int) work_string->length();

  std::cout << "P:" << id << " sending work with " << count <<
    " total MPI::CHARs -- " << *work_string << std::endl;

  MPI::COMM_WORLD.Send(
    (void *) work_string->data(),
    count,
    MPI::CHAR,
    worker_id,
    MW_Master::WORK_TAG
  );

  std::cout << "P:" << id << " finished send to P" << worker_id << ". " <<
    workToDo->size() << " work items remaining" << std::endl;

  // delete work;
  delete work_string;
}

void MW_Master::send_work()
{
  // TODO
}

// void MW_Master::result_probe()
// {
//   MPI::Status status;
//   MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, WORK_TAG, status);
//   std::cout << "P:" << id << " Probe reported process " <<
//     status.Get_source() << " has " << status.Get_count(MPI::CHAR) <<
//     " values" << std::endl;
// }

void MW_Master::receive_result()
{
  std::cout << "P:" << id << " master waiting to receive" << std::endl;

  MPI::Status status;
  // std::string& message = new std::string(1000, 0);
  char *message = (char*)malloc(1000);
  memset(message, 0, 1000);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    1000,
    MPI::CHAR,
    MPI::ANY_SOURCE,
    MW_Master::WORK_TAG,
    status
  );


  std::string serializedObject = std::string(message,status.Get_count(MPI::CHAR));
  free(message);

  int worker_id = status.Get_source();
  std::cout << "P:" << id << " Got data from process " <<
    worker_id << "of count " << status.Get_count(MPI::CHAR) <<
    std::endl;

  Result *result = Result::deserialize(serializedObject);
  results->push_back(result);

  // Reinclude this working in the queue
  workers->push_back(worker_id);
}
