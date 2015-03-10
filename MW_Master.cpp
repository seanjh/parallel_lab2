#include "MW_Master.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <iostream>
#include <list>
#include <string>
#include <mpi.h>
#include <assert.h>

MW_Master::MW_Master(const int myid, const int sz, const std::list<Work *> &work, MW_API *a)
{
  id = myid;
  world_size = sz;
  app = a;

  std::cout << "In master constructor" << std::endl;
  //workToDo = std::list<Work *>();
  std::cout << &work << std::endl;
  std::cout << a << std::endl;

  
  // Work *pieceOfWork = work.front();
  // std::cout<<pieceOfWork <<std::endl;
  // std::cout<<pieceOfWork->serialize() <<std::endl;
  // std::cout << *(a->workSerializer(*pieceOfWork)) <<std::endl;
  for (auto iter = work.begin();
    iter != work.end();
    iter++)
  {
    Work *pieceOfWork;
    std::cout<<"Work Pushing " << *(a->workSerializer(**iter)) << std::endl;
    workToDo.push_back(*iter);
  }
  //results = std::list<Result *>();
  std::cout << "Total work in master is " << workToDo.size() << std::endl;

  // Prepopulate the list of workers
  //workers = std::list<int>();
  for (int i=0; i<world_size; i++) {
    if (i != id) {
      std::cout<<"Worker Pushing " << i << std::endl;
      workers.push_back(i);
    }
  }
  //std::cout <<"Number of workers: " << workers->size() << std::endl;
}

MW_Master::~MW_Master()
{
  for ( auto iter = results.begin();
      iter != results.end();
      iter++)
  {
    Result *result = *iter;
    delete result;
  }

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

    std::cout << "At top of master loop" << std::endl;
    std::cout <<"Number of workers: " << workers.size() << std::endl;
    //if no more work and we have received messages from all of our workers
    //send done message
    if(workToDo.empty() && workers.size() == world_size-1)
    {
      //send done message
      std::cout << "sending done messages " << std::endl;
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
      break;

    }
    //if available workers and work
    //send work to worker
    else if( !workers.empty() && !workToDo.empty())
    {

      int worker_id = workers.front();
      workers.pop_front();

      std::cout << "sending message to " << worker_id << std::endl;

      Work *work = workToDo.front();
      workToDo.pop_front();

      std::string *work_string = work->serialize();


      //send serialized work to worker
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << *work_string << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      MPI::COMM_WORLD.Send(
        (void *) work_string->data(),
        work_string->length(),
        MPI::CHAR,
        worker_id,
        MW_Master::WORK_TAG
      );

      std::cout << "message sent to " << worker_id << std::endl;
      //setup receive for worker
      memset(resultMessages[worker_id-1], 0, MAX_MESSAGE_SIZE);
      std::cout << " receiving from " << worker_id << std::endl;
      requestArray[worker_id-1] = MPI::COMM_WORLD.Irecv(
        (void *) resultMessages[worker_id-1],
        MAX_MESSAGE_SIZE,
        MPI::CHAR,
        worker_id,
        MW_Master::WORK_TAG
      );

      std::cout << "initiated receive from " << worker_id << std::endl;
    }
    //if no available workers and there is available work, wait for all workers
    //receive results

    //if available workers and no work, wait for rest of workers to respond before

    else if ((workers.empty() && !workToDo.empty()) || workToDo.empty())
    {
      //requestId will specify the request id of the message returned
      MPI::Status status;
      int requestId =  MPI::Request::Waitany(world_size-1, requestArray, status);
      assert(requestId >= 0 && requestId < world_size);
      std::cout << "Got result Message from " << requestId+1 << std::endl;
      std::cout << "string length " << status.Get_count(MPI::CHAR) <<std::endl;
      std::string serializedObject = std::string(resultMessages[requestId], status.Get_count(MPI::CHAR));
      std::cout << serializedObject << std::endl;
      Result *result = app->resultDeserializer(serializedObject);
      std::cout << result->serialize() << std::endl;
      results.push_back(result);

      // Reinclude this working in the queue
      std::cout << "Adding working to back of queue " << requestId+1 << std::endl;
      workers.push_back(requestId+1);
    }
     
    else
    {
      std::cout<<"Asserting"<<std::endl;
      assert(1);
    }


  }

  std::cout<<"Deleting a bunch of stuff"<<std::endl;
  delete requestArray[world_size-1];
 
  for(int i=0; i<world_size-1; i++)
    free(resultMessages[i]);



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

//   // deletedelete work;
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
