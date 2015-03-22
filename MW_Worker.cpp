#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"
#include <sstream>
#include "MW_Random.hpp"

MW_Worker::MW_Worker(int myid, int m_id, int size): id(myid), master_id(m_id), world_size(size), preemptionTimer(.1)
{
  // NADA
}

MWTag MW_Worker::receive()
{
  // std::cout << "P:" << id << " waiting to receive work from master." << std::endl;

  MPI::Status status;
  bool can_receive = MPI::COMM_WORLD.Iprobe(
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    status
  );

  if (!can_receive) {
    //std::cout << "IProbe did not find a message. No receive possible\n";
    return NOTHING_TAG;
  }

  // MPI::Status status;
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
    std::string messageString = std::string(message, status.Get_count(MPI::CHAR));
    // std::cout << "P:" << id << " Received from master P" << master_id << " message \"" << serializedObject << "\"\n";

    std::istringstream iss (messageString);
    std::string idString, serializedObject;

    std::getline(iss,idString,',');
    MW_ID work_id = std::stoul(idString);
    // std::cout<<work_id<<std::endl;

    std::getline(iss,serializedObject);
    // std::cout<<serializedObject<<std::endl;

    // MPIMessage *mpi_message = new MPIMessage(serializedObject);
    auto mpi_message = std::make_shared<MPIMessage>(serializedObject);
    // std::cout << "P:" << id << " mpi_message (work) is " << mpi_message->to_string() << std::endl;
    // MPIMessage *mpi_message = new MPIMessage(serializedObject);
    // std::cout << "P:" << id << " mpi_message (work) is " << mpi_message->to_string() << std::endl;

    //Work *work = mpi_message->deserializeWork();
    std::shared_ptr<Work> work = mpi_message->deserializeWork();
    // delete mpi_message;
    assert(work != NULL);
    // std::cout << "P:" << id << " Recreated work object (" << work << ") \"" << *work->serialize() << "\"\n" ;

    //TODO extract ID and add to map
    workToDo[work_id] = work;
    // std::cout << "P:" << id << " WorkToDo size is " << workToDo->size() << std::endl;
  } else {
    // Do nothing
    std::cout << "P" << id << ": Received unknown message (heartbeat?)" << std::endl;
  }

  free(message);
  return static_cast<MWTag>(message_tag);
}

// void MW_Worker::send()
// {
//   // std::cout << "P:" << id << " Beginning send to master P" << master_id << std::endl;
//   auto resultPair = *(results.begin());
//   results.erase(resultPair.first);
//   std::shared_ptr<Result> result = resultPair.second;
//   assert(result);
//   // results->pop_front();

//   // std::cout << " Popped result (" << result << ") from list\n";

//   // std::string result_string = resultPair.first.to_string() + "," + *(result->serialize());
//   std::shared_ptr<std::string> result_string = std::make_shared<std::string> (std::to_string(resultPair.first) + "," + *(result->serialize()));

//   int count = (int) result_string->length();

//   // std::cout << "P:" << id << " sending result with " << count <<
//   //   " total MPI::CHARs -- " << result_string << std::endl;

//   assert(count < MAX_MESSAGE_SIZE);
//   MPI::COMM_WORLD.Send(
//     (void *) result_string->data(),
//     count,
//     MPI::CHAR,
//     master_id,
//     WORK_TAG
//   );

//   // std::cout << "P:" << id << " finished send to master P" << master_id << ". " <<
//   //   results->size() << " results remaining" << std::endl;

//   // delete result;
//   // delete result_string;
// }

void MW_Worker::send(MW_ID result_id, std::shared_ptr<Result> result)
{

  std::shared_ptr<std::string> result_string = \
    std::make_shared<std::string> (std::to_string(result_id) + \
    "," + *(result->serialize()));

  int count = (int) result_string->length();
  MPI::COMM_WORLD.Send(
    (void *) result_string->data(),
    count,
    MPI::CHAR,
    master_id,
    WORK_TAG
  );

}


MW_Worker::~MW_Worker()
{
  // workToDo should be empty, so deleting its elements is bonus caution
  // for ( auto iter = workToDo->begin();
  //     iter != workToDo->end();
  //     iter++)
  // {
  //   Work *work = *iter;
  //   delete work;
  // }

  // delete workToDo;


  // for ( auto iter = results->begin();
  //     iter != results->end();
  //     iter++)
  // {
  //   Result *result = *iter;
  //   delete result;
  // }

  // delete results;
}

// void MW_Worker::doWork()
// {
//   // std::cout << "P:" << worker->id << " Grabbing some workToDo\n";
//   // Work *work = workToDo->front();
//   // assert(work != NULL);
//   // // std::cout << "P:" << worker->id << " Work object is (" << work << ") \"" << work->serialize() << "\"\n";
//   // workToDo->pop_front();
//   auto workPair = *(workToDo.begin());
//   MW_ID work_id = workPair.first;
//   // std::cout <<work_id<< ": "<< *(workPair.second->serialize()) <<std::endl;
//   workToDo.erase(work_id);
//   std::shared_ptr<Work> work = workPair.second;

//   // std::cout << "P:" << worker->id << " Computing results.\n";

//   MW_API_STATUS_CODE status;
//   do
//   {
//     // std::cout << "Entered Loop" << std:: endl;
//     status = work->compute(preemptionSemaphore);

//   } while(status == Preempted);

//   // std::cout << "Exited Loop" << std:: endl;
//   std::shared_ptr<Result> new_result = work->result();
//   // std::cout << new_result << std:: endl;
//   // std::cout << "generated results" << std:: endl;

//   assert(new_result);
//   // std::cout << "P:" << worker->id << " Result object is (" << one_result << ") \"" << one_result->serialize() << "\"\n";

//   // TODO compute ID and add to
//   // results->push_back(new_result);
//   results[work_id] = new_result;
// }

void MW_Worker::worker_loop()
{
  // Result *result;

  MW_Random random = MW_Random(WORKER_FAILURE_PROBABILITY, id, world_size);
  std::cout << "P" << id << ": ";
  bool will_fail = random.random_fail();
  if (will_fail && WORKER_FAIL_TEST_ON) {
    std::cout << "P:" << id << " THIS WORKER WILL EVENTUALLY FAIL\n";
  }
  random = MW_Random(id, world_size);

  MWTag message_tag;
  while (1) {

    if (random.random_fail() && WORKER_FAIL_TEST_ON) {
      std::cout << "P:" << id << " WORKER FAILURE EVENT\n";
      MPI::Finalize();
      exit (0);
    }

    message_tag = receive();

    preemptionTimer.reset();


    if (message_tag == WORK_TAG) {

      // std::cout<<"Received WorkTag"<<std::endl;
      // doWork();
      // send();

    } else if (message_tag == DONE_TAG) {
      // std::cout << "P:" << proc->id << " IS DONE\n";
      break;
    } else if (message_tag == HEARTBEAT_TAG) {
      // std::cout << "P:" << proc->id << " IS DONE\n";
      continue;
    } else if (hasWork())
    {
      auto workPair = *(workToDo.begin());
      MW_ID work_id = workPair.first;

      std::shared_ptr<Work> work = workPair.second;

      MW_API_STATUS_CODE status;
      status = work->compute(preemptionTimer);
      if (status != Preempted)
      {
        // std::cout <<"Worker returning Success"<<std::endl;
        assert(status == Success);

        //remove from work to do map
        workToDo.erase(work_id);
        std::shared_ptr<Result> new_result = work->result();

        assert(new_result);
        results[work_id] = new_result;

        //send results back to master
        send(work_id, new_result);

      }
      else
      {
        // std::cout <<"Worker returning preempted"<<std::endl;
        continue;
      }

    }
    else {
      // std::cout << "WTF happened here\n";
      // assert(0);
      continue;
    }
  }
}

// void MW_Worker::preemptionTimerHandler(const boost::system::error_code& error)
// {
//   // if (!error)
//   // {
//   //   std::cout<<"Timer Expired. Resetting." << std::endl;
//   //   preemptionTimer = std::unique_ptr<boost::asio::deadline_timer> \
//   //   (new boost::asio::deadline_timer(
//   //     *preemptionTimerIOService,
//   //     boost::posix_time::milliseconds(100)));
//   //   preemptionTimer->async_wait(this->preemptionTimerHandler);
//   // }
// }
