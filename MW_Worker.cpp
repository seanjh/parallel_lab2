#include "MW_Worker.hpp"
#include <mpi.h>

MW_Worker::MW_Worker(const int myid, const int m_id)
{
  id = myid;
  master_id = m_id;

  // Blanks lists for work and results
  workToDo = new std::list<Work *>();
  results = new std::list<Result *>();

  std::cout << "" << std::endl;
}

void MW_Worker::receiveWork()
{
  std::cout << "P:" << id << " waiting to receive work from master." <<
    std::endl;

  MPI::Status status;
  char *message = (char*)malloc(1000);
  memset(message, 0, 1000);
  // std::string message(1000, 0);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    1000,
    MPI::CHAR,
    master_id,
    MW_Worker::WORK_TAG,
    status
  );

  std::string serializedObject = std::string(message, status.Get_count(MPI::CHAR));
  free(message);

  Work *work = Work::deserialize(serializedObject);
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
