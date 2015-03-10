#include "MW_Worker.hpp"
#include <mpi.h>
#include <assert.h>

MW_Worker::MW_Worker(const int myid, const int m_id, MW_API *mwapp)
{
  id = myid;
  master_id = m_id;
  app = mwapp;

  // Blanks lists for work and results
  workToDo = new std::list<Work *>();
  results = new std::list<Result *>();

  std::cout << "" << std::endl;
}

int MW_Worker::receiveWork()
{
  std::cout << "P:" << id << " waiting to receive work from master." << std::endl;

  MPI::Status status;
  char *message = (char*)malloc(1000);
  memset(message, 0, 1000);
  // std::string message(1000, 0);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    1000,
    MPI::CHAR,
    master_id,
    MPI::ANY_TAG,
    // MW_Worker::WORK_TAG,
    status
  );

  int message_tag = status.Get_tag();

  if (message_tag == MW_Worker::WORK_TAG) {
    std::string serializedObject = std::string(message, status.Get_count(MPI::CHAR));
    std::cout << "P:" << id << " Received from master P" << master_id << " message \"" << serializedObject << "\"\n";

    Work *work = app->workDeserializer(serializedObject);
    assert(work != NULL);
    std::cout << "P:" << id << " Recreated work object (" << work << ") \"" << *work->serialize() << "\"\n" ;

    workToDo->push_back(work);
    std::cout << "P:" << id << " WorkToDo size is " << workToDo->size() << std::endl;

  }

  free(message);
  return message_tag;
}

void MW_Worker::sendResults()
{
  std::cout << "P:" << id << " Beginning send to master P" << master_id << std::endl;
  Result *result = results->front();
  assert(result != NULL);
  results->pop_front();

  std::cout << " Popped result (" << result << ") from list\n";

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
