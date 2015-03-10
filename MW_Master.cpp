#include "MW_Master.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <iostream>
#include <list>
#include <string>
#include <mpi.h>
#include <assert.h>

MW_Master::MW_Master(const int myid, const int sz, std::list<Work *> *work, MW_API *app)
{
  id = myid;
  world_size = sz;
  app = app;

  workToDo = new std::list<Work *>(*(work));
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

MW_Master::~MW_Master()
{
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

void MW_Master::masterLoop()
{
  std::cout << "In master loop" << std::endl;

  //create requests for each worker to return work
  MPI::Request *requestArray = new MPI::Request[world_size-1];
  char **resultMessages;
 
  for(int i=0; i<world_size-1; i++)
    resultMessages[i]  = (char*)malloc(MAX_MESSAGE_SIZE);


  while(1)
  {

    //if no more work and we have received messages from all of our workers
    //send done message
    if(workToDo->empty() && workers->size() == world_size)
    {
      //send done message
      for(int i=1; i<world_size; i++)
      {
        char a = 'a';
        MPI::COMM_WORLD.Send(
          (void*)&a,
          1,
          MPI::CHAR,
          i,
          MW_Master::DONE_TAG
        );
      }

    }
    //if available workers and work
    //send work to worker
    else if( !workers->empty() && !workToDo->empty())
    {

      int worker_id = workers->front();
      workers->pop_front();

      std::cout << "sending message to " << worker_id << std::endl;

      Work *work = workToDo->front();
      workToDo->pop_front();

      std::string *work_string = work->serialize();

      //send serialized work to worker
      MPI::COMM_WORLD.Send(
        (void *) work_string->data(),
        work_string->length(),
        MPI::CHAR,
        worker_id,
        MW_Master::WORK_TAG
      );

      //setup receive for worker
      memset(resultMessages[worker_id-1], 0, MAX_MESSAGE_SIZE);
      requestArray[worker_id-1] = MPI::COMM_WORLD.Irecv(
        (void *) resultMessages[worker_id-1],
        MAX_MESSAGE_SIZE,
        MPI::CHAR,
        id,
        MW_Master::WORK_TAG
      );
    }
    //if no available workers and there is available work, wait for all workers
    //receive results

    //if available workers and no work, wait for rest of workers to respond before

    else if ((workers->empty() && !workToDo->empty()) || workToDo->empty())
    {
      //requestId will specify the request id of the message returned
      MPI::Status status;
      int requestId =  MPI::Request::Waitany(world_size-1, requestArray, status);

      std::cout << "Got result Message" << std::endl;
      std::string serializedObject = std::string(resultMessages[requestId], status.Get_count(MPI::CHAR));
      std::cout << serializedObject << std::endl;
      Result *result = app->resultDeserializer(serializedObject);
      results->push_back(result);

      // Reinclude this working in the queue
      workers->push_back(requestId+1);
    }
     
    else
    {
      std::cout<<"Asserting"<<std::endl;
      assert(1);
    }


  }

}

// void MW_Master::send_one(int worker_id)
// {
//   std::cout << "P:" << this->id << " sending work to process " << worker_id << std::endl;

//   Work *work = workToDo->front();
//   workToDo->pop_front();

//   std::string *work_string = work->serialize();
//   int count = (int) work_string->length();

//   std::cout << "P:" << id << " sending work with " << count <<
//     " total MPI::CHARs -- " << *work_string << std::endl;

//   MPI::COMM_WORLD.Send(
//     (void *) work_string->data(),
//     count,
//     MPI::CHAR,
//     worker_id,
//     MW_Master::WORK_TAG
//   );

//   std::cout << "P:" << id << " finished send to P" << worker_id << ". " <<
//     workToDo->size() << " work items remaining" << std::endl;

//   // delete work;
//   delete work_string;
// }

// void MW_Master::send_work()
// {
//   // TODO
// }

// // void MW_Master::result_probe()
// // {
// //   MPI::Status status;
// //   MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, WORK_TAG, status);
// //   std::cout << "P:" << id << " Probe reported process " <<
// //     status.Get_source() << " has " << status.Get_count(MPI::CHAR) <<
// //     " values" << std::endl;
// // }

// void MW_Master::receive_result()
// {
//   std::cout << "P:" << id << " master waiting to receive" << std::endl;

//   MPI::Status status;
//   // std::string& message = new std::string(1000, 0);
//   char *message = (char*)malloc(1000);
//   memset(message, 0, 1000);

//   MPI::COMM_WORLD.Recv(
//     (void *) message,
//     1000,
//     MPI::CHAR,
//     MPI::ANY_SOURCE,
//     MW_Master::WORK_TAG,
//     status
//   );


//   std::string serializedObject = std::string(message,status.Get_count(MPI::CHAR));
//   free(message);

//   int worker_id = status.Get_source();
//   std::cout << "P:" << id << " Got data from process " <<
//     worker_id << "of count " << status.Get_count(MPI::CHAR) <<
//     std::endl;

//   Result *result = Result::deserialize(serializedObject);
//   results->push_back(result);

//   // Reinclude this working in the queue
//   workers->push_back(worker_id);
// }
