#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"
#include <sstream>

// <<<<<<< HEAD
MW_Worker::MW_Worker(const int myid, const int m_id, const int w_size): preemptionTimer(.1), random(MW_Random(myid, w_size))
{
  id = myid;
  master_id = m_id;
  world_size = w_size;
// =======
// MW_Worker::MW_Worker(int myid, int m_id, int size): id(myid), master_id(m_id), world_size(size), preemptionTimer(.1)
// {
//   // NADA
// >>>>>>> sean-lab3
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
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    // MW_Worker::WORK_TAG,
    status
  );

  // int message_tag = status.Get_tag();

  MWTag message_tag = static_cast<MWTag>(status.Get_tag());
  int source_id = status.Get_source();
  int count = status.Get_count(MPI::CHAR);

  // std::cout<< "received message" <<std::endl;

  if (message_tag == WORK_TAG) {
    assert(source_id == master_id);
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
  } else if (message_tag == HEARTBEAT_TAG) {
    // std::cout << "Received heartbeat from " << source_id << std::endl;

    if (source_id == master_id)
    {
      if (!masterMonitor)
        masterMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD));
      masterMonitor->addHeartbeat();
    }
    else
    {
      std::shared_ptr<MW_Monitor> nodeMonitor;
      try {
        nodeMonitor = otherWorkersMonitorMap.at(source_id);
      }
      catch (const std::out_of_range& oor) {
        
        std::cout << "Received first heartbeat from " << source_id << ". Adding to monitor map\n";
        nodeMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD));
        otherWorkersMonitorMap[source_id] = nodeMonitor;
      }
      nodeMonitor->addHeartbeat();
    }
  }
  // else
  // {
  //   // Do nothing
  //   //std::cout << "Received unknown message" << std::endl;
  // }

  free(message);
  return message_tag;
}


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

}

void MW_Worker::worker_loop()
{
  // Result *result;
// <<<<<<< HEAD
//   // MW_Random random = MW_Random(id, 1);
// =======

//   MW_Random random = MW_Random(WORKER_FAILURE_PROBABILITY, id, world_size);
//   std::cout << "P" << id << ": ";
//   bool will_fail = random.random_fail();
//   if (will_fail && WORKER_FAIL_TEST_ON) {
//     std::cout << "P:" << id << " THIS WORKER WILL EVENTUALLY FAIL\n";
//   }
//   random = MW_Random(id, world_size);
// >>>>>>> sean-lab3

  MWTag message_tag;
  while (1) {

    // if (random.random_fail() && WORKER_FAIL_TEST_ON) {
    //   std::cout << "P:" << id << " WORKER FAILURE EVENT\n";
    //   MPI::Finalize();
    //   exit (0);
    // }

    // std::cout << "Top of worker loop" <<std::endl;

    // if(!masterMonitor && shouldSendHeartbeat())
    // {
    //   // int worker_array[16];
    //   // MPI::COMM_WORLD.Bcast(
    //   //   (void *)&world_size, 
    //   //   16,
    //   //   MPI::INT 
    //   //   master_id);

    //   // masterMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD));
    //   broadcastHeartbeat();
    // }

    if(shouldSendHeartbeat()) 
    {
      sendHeartbeat();
      continue;
    }

    message_tag = receive();

    // if(shouldSendHeartbeat()) 
    // {
    //   sendHeartbeat();
    //   continue;
    // }

    preemptionTimer.reset();

    if (message_tag == WORK_TAG) {

      // std::cout<<"Received WorkTag"<<std::endl;
      // doWork();
      // send();

    } else if (message_tag == DONE_TAG) {
      std::cout << "P:" << id << " IS DONE\n";
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

bool MW_Worker::shouldSendHeartbeat()
{
  return (MPI::Wtime() - lastHeartbeat) > HEARTBEAT_PERIOD;
}

// void MW_Worker::sendHeartbeat()
// {
//   std::cout << "worker sending heartbeat" <<std::endl;
//   // if(masterMonitor->isAlive())
//   masterMonitor->sendHeartbeat(false);
//   std::cout << "worker sent heartbeat to master" <<std::endl;
//   for(auto it=otherWorkersMonitorMap.begin(); it != otherWorkersMonitorMap.end(); it++)
//   {
//     // if (it->second->isAlive())
//     it->second->sendHeartbeat(true);
//   }
//   lastHeartbeat = MPI::Wtime();
//   std::cout << "worker finished sending heartbeat" <<std::endl;
// }

void MW_Worker::sendHeartbeat()
{
  if (random.random_fail()) {
    std::cout << "P:" << id << " WORKER FAILURE EVENT\n";
    MPI::Finalize();
    exit (0);
  }

  if (!masterMonitor)
    broadcastHeartbeat();
  else 
  {
    // std::cout << "worker sending heartbeat" <<std::endl;
    // if(masterMonitor->isAlive())
    masterMonitor->sendHeartbeat(false);
    // std::cout << "worker sent heartbeat to master" <<std::endl;
    for(auto it=otherWorkersMonitorMap.begin(); it != otherWorkersMonitorMap.end(); it++)
    {
      // if (it->second->isAlive())
      it->second->sendHeartbeat(false);
    }
    lastHeartbeat = MPI::Wtime();
    // std::cout << "worker finished sending heartbeat" <<std::endl;
  }
  
}


void MW_Worker::broadcastHeartbeat()
{
  // std::cout << "Worker Broadcasting heartbeat" << std::endl;
  for (int i=0; i<world_size; i++)
  {
    if (i != id)
    {
      // std::cout<<"Sending heartbeat to " << i <<std::endl;
      char m = 0;
      MPI::COMM_WORLD.Send(
        (void *) &m,
        1,
        MPI::CHAR,
        i,
        HEARTBEAT_TAG
      );
      // std::cout<<"Sent heartbeat to " << id <<std::endl;
    }
  }
  // std::cout<<"Worker done sending heartbeats"<<std::endl;
  lastHeartbeat = MPI::Wtime();
}
