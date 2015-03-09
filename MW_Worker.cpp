#include "MW_Worker.hpp"
#include <mpi.h>

MW_Worker::MW_Worker(const int myid, const int m_id)
{
  id = myid;
  master_id = m_id;

  // Blanks lists for work and results
  workToDo = new std::list<Work *>();
  results = new std::list<Result *>();
}

void MW_Worker::receiveWork()
{
  std::cout << "P:" << id << " waiting to receive work from master." <<
    std::endl;

  MPI::Status status;
  std::string message(1000, 0);

  MPI::COMM_WORLD.Recv(
    (void *) message.data(),
    1000,
    MPI::CHAR,
    master_id,
    MW_Worker::WORK_TAG,
    status
  );

  Work *work = Work::deserialize(message);
  workToDo->push_back(work);

  std::cout << "P:" << id << " finished receive from master P" << master_id << ". "
    << "WorkToDo size is " << workToDo->size() << std::endl;
}

void MW_Worker::sendResults()
{
  Result *result = results->front();
  results->pop_front();

  std::string *result_string = result->serialize();
  int count = (int) result_string->length();

  std::cout << "P:" << id << " sending result with " << count <<
    " total MPI::CHARs -- " << result_string << std::endl;

  MPI::COMM_WORLD.Send(
    (void *) result_string->data(),
    count,
    MPI::CHAR,
    master_id,
    MW_Worker::WORK_TAG
  );

  std::cout << "P:" << id << " finished send to master P" << master_id << ". " <<
    results->size() << " results remaining" << std::endl;

  // delete result;
  delete result_string;
}
