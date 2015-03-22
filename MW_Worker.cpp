#include <iostream>
#include <sstream>
#include <assert.h>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"
#include "MW_Random.hpp"

using namespace std;

MW_Worker::MW_Worker(int myid, int m_id, int size): id(myid), master_id(m_id), world_size(size), preemptionTimer(.1)
{
  // NADA
}

MWTag MW_Worker::receive()
{
  // cout << "P:" << id << " waiting to receive work from master." << endl;

  MPI::Status status;
  bool can_receive = MPI::COMM_WORLD.Iprobe(
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    status
  );

  if (!can_receive) {
    //cout << "IProbe did not find a message. No receive possible\n";
    return NOTHING_TAG;
  }

  // MPI::Status status;
  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  // string message(1000, 0);

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
    string messageString = string(message, status.Get_count(MPI::CHAR));
    // cout << "P:" << id << " Received from master P" << master_id << " message \"" << serializedObject << "\"\n";

    istringstream iss (messageString);
    string idString, serializedObject;

    getline(iss,idString,',');
    MW_ID work_id = stoul(idString);
    // cout<<work_id<<endl;

    getline(iss,serializedObject);
    // cout<<serializedObject<<endl;

    // MPIMessage *mpi_message = new MPIMessage(serializedObject);
    auto mpi_message = make_shared<MPIMessage>(serializedObject);
    // cout << "P:" << id << " mpi_message (work) is " << mpi_message->to_string() << endl;
    // MPIMessage *mpi_message = new MPIMessage(serializedObject);
    // cout << "P:" << id << " mpi_message (work) is " << mpi_message->to_string() << endl;

    //Work *work = mpi_message->deserializeWork();
    shared_ptr<Work> work = mpi_message->deserializeWork();
    // delete mpi_message;
    assert(work != NULL);
    // cout << "P:" << id << " Recreated work object (" << work << ") \"" << *work->serialize() << "\"\n" ;

    //TODO extract ID and add to map
    workToDo[work_id] = work;
    // cout << "P:" << id << " WorkToDo size is " << workToDo->size() << endl;
  } else {
    // Do nothing
    cout << "P" << id << ": Received unknown message (heartbeat?)" << endl;
  }

  free(message);
  return static_cast<MWTag>(message_tag);
}

// void MW_Worker::send()
// {
//   // cout << "P:" << id << " Beginning send to master P" << master_id << endl;
//   auto resultPair = *(results.begin());
//   results.erase(resultPair.first);
//   shared_ptr<Result> result = resultPair.second;
//   assert(result);
//   // results->pop_front();

//   // cout << " Popped result (" << result << ") from list\n";

//   // string result_string = resultPair.first.to_string() + "," + *(result->serialize());
//   shared_ptr<string> result_string = make_shared<string> (to_string(resultPair.first) + "," + *(result->serialize()));

//   int count = (int) result_string->length();

//   // cout << "P:" << id << " sending result with " << count <<
//   //   " total MPI::CHARs -- " << result_string << endl;

//   assert(count < MAX_MESSAGE_SIZE);
//   MPI::COMM_WORLD.Send(
//     (void *) result_string->data(),
//     count,
//     MPI::CHAR,
//     master_id,
//     WORK_TAG
//   );

//   // cout << "P:" << id << " finished send to master P" << master_id << ". " <<
//   //   results->size() << " results remaining" << endl;

//   // delete result;
//   // delete result_string;
// }

void MW_Worker::send(MW_ID result_id, shared_ptr<Result> result)
{

  shared_ptr<string> result_string = \
    make_shared<string> (to_string(result_id) + \
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
//   // cout << "P:" << worker->id << " Grabbing some workToDo\n";
//   // Work *work = workToDo->front();
//   // assert(work != NULL);
//   // // cout << "P:" << worker->id << " Work object is (" << work << ") \"" << work->serialize() << "\"\n";
//   // workToDo->pop_front();
//   auto workPair = *(workToDo.begin());
//   MW_ID work_id = workPair.first;
//   // cout <<work_id<< ": "<< *(workPair.second->serialize()) <<endl;
//   workToDo.erase(work_id);
//   shared_ptr<Work> work = workPair.second;

//   // cout << "P:" << worker->id << " Computing results.\n";

//   MW_API_STATUS_CODE status;
//   do
//   {
//     // cout << "Entered Loop" <<  endl;
//     status = work->compute(preemptionSemaphore);

//   } while(status == Preempted);

//   // cout << "Exited Loop" <<  endl;
//   shared_ptr<Result> new_result = work->result();
//   // cout << new_result <<  endl;
//   // cout << "generated results" <<  endl;

//   assert(new_result);
//   // cout << "P:" << worker->id << " Result object is (" << one_result << ") \"" << one_result->serialize() << "\"\n";

//   // TODO compute ID and add to
//   // results->push_back(new_result);
//   results[work_id] = new_result;
// }

void MW_Worker::worker_loop()
{
  // Result *result;

  MW_Random random = MW_Random(WORKER_FAILURE_PROBABILITY, id, world_size);
  cout << "P" << id << ": ";
  bool will_fail = random.random_fail();
  if (will_fail && WORKER_FAIL_TEST_ON) {
    cout << "P:" << id << " THIS WORKER WILL EVENTUALLY FAIL\n";
  }
  random = MW_Random(id, world_size);

  MWTag message_tag;
  while (1) {

    if (random.random_fail() && WORKER_FAIL_TEST_ON) {
      cout << "P:" << id << " WORKER FAILURE EVENT\n";
      MPI::Finalize();
      exit (0);
    }

    message_tag = receive();

    preemptionTimer.reset();


    if (message_tag == WORK_TAG) {

      // cout<<"Received WorkTag"<<endl;
      // doWork();
      // send();

    } else if (message_tag == DONE_TAG) {
      // cout << "P:" << proc->id << " IS DONE\n";
      break;
    } else if (message_tag == HEARTBEAT_TAG) {
      // cout << "P:" << proc->id << " IS DONE\n";
      continue;
    } else if (hasWork())
    {
      auto workPair = *(workToDo.begin());
      MW_ID work_id = workPair.first;

      shared_ptr<Work> work = workPair.second;

      MW_API_STATUS_CODE status;
      status = work->compute(preemptionTimer);
      if (status != Preempted)
      {
        // cout <<"Worker returning Success"<<endl;
        assert(status == Success);

        //remove from work to do map
        workToDo.erase(work_id);
        shared_ptr<Result> new_result = work->result();

        assert(new_result);
        results[work_id] = new_result;

        //send results back to master
        send(work_id, new_result);

      }
      else
      {
        // cout <<"Worker returning preempted"<<endl;
        continue;
      }

    }
    else {
      // cout << "WTF happened here\n";
      // assert(0);
      continue;
    }
  }
}

// void MW_Worker::preemptionTimerHandler(const boost::system::error_code& error)
// {
//   // if (!error)
//   // {
//   //   cout<<"Timer Expired. Resetting." << endl;
//   //   preemptionTimer = unique_ptr<boost::asio::deadline_timer> \
//   //   (new boost::asio::deadline_timer(
//   //     *preemptionTimerIOService,
//   //     boost::posix_time::milliseconds(100)));
//   //   preemptionTimer->async_wait(this->preemptionTimerHandler);
//   // }
// }
