#include "MW_Worker.hpp"
#include <iostream>
#include <mpi.h>



MW_Worker::MW_Worker(const int myid, const int m_id, MW_API *app)
{
  id = myid;
  master_id = m_id;
  app = app;

  // Blanks lists for work and results
  workToDo = new std::list<Work *>();
  results = new std::list<Result *>();

  std::cout << "" << std::endl;
}

void MW_Worker::workerLoop()
{
  

  MPI::Status status;
  char *workMessage = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(workMessage, 0, MAX_MESSAGE_SIZE);
  MPI::Request requestArray[2];

  //receive work
  requestArray[0] = MPI::COMM_WORLD.Irecv(
    (void *) workMessage,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    master_id,
    MW_Worker::WORK_TAG
  );

  char *doneMessage = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(doneMessage, 0, MAX_MESSAGE_SIZE);
  //receive done
  requestArray[1] = MPI::COMM_WORLD.Irecv(
    (void *) doneMessage,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    master_id,
    MW_Worker::DONE_TAG
  );

  while(1)
  {

    std::cout << "In worker loop" << std::endl;
    //requestId will specify the request id of the message returned
    int requestId =  MPI::Request::Waitany(2, requestArray, status);
    
    //work message received
    if(requestId == 0)
    {

      std::cout << "Got work Message" << std::endl;
      std::string serializedObject = std::string(workMessage, status.Get_count(MPI::CHAR));
      std::cout <<id << ": "<< serializedObject << std::endl;


      Work *work = app->workDeserializer(serializedObject);
      std::cout << work << std::endl;
      std::cout << "deserialized " << *(work->serialize()) << std::endl;

      std::cout << "computing results " << std::endl;
      Result *result = work->compute();
      std::cout << "computed results " << std::endl;
      std::string *result_string = result->serialize();

      std::cout << "produced results " << *result_string << std::endl;

      MPI::COMM_WORLD.Send(
        (void *) result_string->data(),
        result_string->length(),
        MPI::CHAR,
        master_id,
        MW_Worker::WORK_TAG
      );

      //free objects
      delete(work);
      delete(result);
      delete(result_string);

      //set the receive back up
      memset(workMessage, 0, MAX_MESSAGE_SIZE);
      requestArray[0] = MPI::COMM_WORLD.Irecv(
        (void *) workMessage,
        MAX_MESSAGE_SIZE,
        MPI::CHAR,
        master_id,
        MW_Worker::WORK_TAG
      );
    }
    //done message received
    else if(requestId == 1)
    {
      std::cout << "Got done Message" << std::endl;
      break;
    }
    else
    {
      std::cout << "ERROR" << std::endl;
      break;
    }
  }
}

// void MW_Worker::receiveWork()
// {
//   std::cout << "P:" << id << " waiting to receive work from master." <<
//     std::endl;

//   MPI::Status status;
//   char *message = (char*)malloc(1000);
//   memset(message, 0, 1000);
//   // std::string message(1000, 0);

//   MPI::COMM_WORLD.Recv(
//     (void *) message,
//     1000,
//     MPI::CHAR,
//     master_id,
//     MW_Worker::WORK_TAG,
//     status
//   );

//   std::string serializedObject = std::string(message, status.Get_count(MPI::CHAR));
//   free(message);

//   Work *work = Work::deserialize(serializedObject);
//   workToDo->push_back(work);

//   std::cout << "P:" << id << " finished receive from master P" << master_id << ". "
//     << "WorkToDo size is " << workToDo->size() << std::endl;
// }

// void MW_Worker::sendResults()
// {
//   Result *result = results->front();
//   results->pop_front();

//   std::string *result_string = result->serialize();
//   int count = (int) result_string->length();

//   std::cout << "P:" << id << " sending result with " << count <<
//     " total MPI::CHARs -- " << result_string << std::endl;

//   MPI::COMM_WORLD.Send(
//     (void *) result_string->data(),
//     count,
//     MPI::CHAR,
//     master_id,
//     MW_Worker::WORK_TAG
//   );

//   std::cout << "P:" << id << " finished send to master P" << master_id << ". " <<
//     results->size() << " results remaining" << std::endl;

//   // delete result;
//   delete result_string;
// }
