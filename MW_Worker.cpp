#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"
#include <sstream>

MW_Worker::MW_Worker(const int myid, const int m_id, const int w_size): preemptionTimer(.1), random(MW_Random(myid, w_size))
{
  id = myid;
  master_id = m_id;
  world_size = w_size;
  heardFromMaster = false;
  waitingForNewMaster = false;
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

  if (can_receive) {
    //std::cout << "IProbe did not find a message. No receive possible\n";
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
      status
    );

    MWTag message_tag = static_cast<MWTag>(status.Get_tag());
    int source_id = status.Get_source();
    int count = status.Get_count(MPI::CHAR);
    // std::cout<< "received message" <<std::endl;

    if (message_tag == WORK_TAG && !waitingForNewMaster) {

      process_work(message, source_id, count);

    } else if (message_tag == HEARTBEAT_TAG) {

      process_heartbeat(source_id);

    } else if (message_tag == NEW_MASTER_TAG) {

      process_new_master(source_id);

    }
    // else
    // {
    //   // Do nothing
    //   //std::cout << "Received unknown message" << std::endl;
    // }

    free(message);
    return message_tag;
  }

  return NOTHING_TAG;
}

void MW_Worker::process_new_master(int source_id)
{
  if (source_id != next_master_id) {

    std::cout << "P" << id << ": UH-OH! Wrong MASTER P" << source_id << " sent NEW_MASTER_TAG!\n";
    MPI::Finalize();
    exit (1);

  } else {

    std::cout << "P" << id << ": HUZZAH! Received welcome from new Master P" << source_id << std::endl;
    master_id = next_master_id;

  }

  waitingForNewMaster = false;
}

void MW_Worker::process_work(char *message, int source_id, int count)
{
  assert(masterMonitor);
  assert(source_id == master_id);
  std::string messageString = std::string(message, count);
  // std::cout << "P:" << id << " Received from master P" << master_id << " message \"" << serializedObject << "\"\n";

  std::istringstream iss (messageString);
  std::string idString, serializedObject;

  // Pull off the prepended work MW_ID
  std::getline(iss,idString,',');
  MW_ID work_id = std::stoul(idString);
  // std::cout<<work_id<<std::endl;

  // Pull off the work message
  std::getline(iss,serializedObject);
  // std::cout<<serializedObject<<std::endl;

  auto mpi_message = std::make_shared<MPIMessage>(serializedObject);
  // std::cout << "P:" << id << " mpi_message (work) is " << mpi_message->to_string() << std::endl;

  std::shared_ptr<Work> work = mpi_message->deserializeWork();
  assert(work != NULL);
  // std::cout << "P:" << id << " Recreated work object (" << work << ") \"" << *work->serialize() << "\"\n" ;

  workToDo[work_id] = work;
  // std::cout << "P:" << id << " WorkToDo size is " << workToDo->size() << std::endl;
}

void MW_Worker::process_heartbeat(int source_id)
{
  // std::cout << "Received heartbeat from " << source_id << std::endl;
  if (source_id == master_id)
  {
    if (!masterMonitor)
      masterMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD));
    masterMonitor->addHeartbeat();
    updateMasterCheckTime();
  }
  else
  {
    std::shared_ptr<MW_Monitor> nodeMonitor;
    try {
      nodeMonitor = otherWorkersMonitorMap.at(source_id);
    }
    catch (const std::out_of_range& oor) {

      std::cout << "P:" << id << " Received its first heartbeat from " << source_id << ". Adding to monitor map\n";
      nodeMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD));
      otherWorkersMonitorMap[source_id] = nodeMonitor;
    }
    nodeMonitor->addHeartbeat();
  }
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


MW_Worker::~MW_Worker() { }

bool MW_Worker::worker_loop()
{
  MWTag message_tag;
  bool done = false;

  while (1) {

    if(shouldSendHeartbeat())
    {
      sendHeartbeat();
      continue;
    }

    if (shouldCheckOnMaster())
    {
      // bool transitionToMaster = checkOnMaster();
      std::cout << "P:" << id << " Checking on Master\n";
      // updateMasterCheckTime();

      bool is_master_alive = isMasterAlive();
      if (is_master_alive) {
        std::cout << "P:" << id << " Master looks OK!\n";
        continue;
      } else {
        std::cout << "P:" << id << " MASTER LOOKS DEAD\n";
        break;
      }
    }

    message_tag = receive();

    preemptionTimer.reset();

    if (message_tag == NEW_MASTER_TAG) {

      continue;

    } else if (message_tag == WORK_TAG) {

      // std::cout<<"Received WorkTag"<<std::endl;
      // doWork();
      // send();
      continue;

    } else if (message_tag == DONE_TAG) {

      std::cout << "P:" << id << " IS DONE\n";
      done = true;
      break;
      // return false;

    } else if (message_tag == HEARTBEAT_TAG) {

      continue;

    } else if (hasWork()) {
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

  return done;
}

bool MW_Worker::shouldSendHeartbeat()
{
  return (MPI::Wtime() - lastHeartbeat) > HEARTBEAT_PERIOD;
}

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

// bool MW_Worker::hasMaster()
// {
//   return masterMonitor;
// }

bool MW_Worker::shouldCheckOnMaster()
{
  // return false;
  // return (masterMonitor != NULL && MPI::Wtime() < masterMonitor->nextHeartbeat());
  // return (masterMonitor != NULL && MPI::Wtime() > nextMasterCheckTime);
  return heardFromMaster && MPI::Wtime() > nextMasterCheckTime;
}

int MW_Worker::findNextMasterId()
{
  int lowest_other_worker_id = world_size - 1;
  // std::unordered_map<int, std::shared_ptr<MW_Monitor>> otherWorkersMonitorMap;
  for ( auto& it: otherWorkersMonitorMap ) {
    if (it.first < lowest_other_worker_id && it.second->isAlive()) {
      lowest_other_worker_id = it.first;
    }
  }

  return id < lowest_other_worker_id ? id : lowest_other_worker_id;
}

void MW_Worker::updateMasterCheckTime()
{
  heardFromMaster = true;

  if (!nextMasterCheckTime) {
    std::cout << "P" << id << ": initializing nextMasterCheckTime\n";
    nextMasterCheckTime = MPI::Wtime();
  }

  nextMasterCheckTime += (HEARTBEAT_PERIOD * 5);
  // nextMasterCheckTime = MPI::Wtime() + (HEARTBEAT_PERIOD * 5);
}

bool MW_Worker::isMasterAlive()
{
  assert(masterMonitor);
  if(masterMonitor->isAlive()) {
    return true;
  }
  return false;
}

void MW_Worker::waitForNewMaster()
{
  // auto next_master_monitor = otherWorkersMonitorMap[next_master_id];
  masterMonitor = nullptr;
  heardFromMaster = false;

  otherWorkersMonitorMap.erase(next_master_id);
  std::cout << "P" << id << ": Monitoring " << otherWorkersMonitorMap.size() <<  " remaining workers: ";
  // for ( auto& x: otherWorkersMonitorMap )
  //   std::cout << "\t" << x.first << " ";
  //   std::cout << "\n";

  std::cout << "P" << id << ": Waiting to hear from the new Master\n";
  waitingForNewMaster = true;
}

bool MW_Worker::transitionMaster()
{
  std::cout << "P" << id << ": Beginning transitions to next Master\n";
  next_master_id = findNextMasterId();
  bool is_next_master = next_master_id == id;

  if (is_next_master) {

    std::cout << "P" << id << ": I AM THE NEXT MASTER\n";

  } else {

    std::cout << "P" << id << ": P" << next_master_id << " is the next MASTER\n";
    waitForNewMaster();

  }

  return is_next_master;
}

// bool MW_Worker::checkOnMaster()
// {
//
//
//   assert(masterMonitor);
//   if(!masterMonitor->isAlive())
//   {
//
//     //flush current work
//
//     //determine which worker is next master by which workers are still alive
//     int next_master = findNextMasterId();
//
//     //if I am the new master, return true
//     if (next_master == id) {
//
//       return true;
//     }
//
//     //if I am not the new master, move the new master off the worker monitor
//     //map into the master slot, set next master check for some long time in the future
//     //return false
//     // std::cout << "P" << id << ": P" << next_master << " is the next MASTER\n";
//     // auto next_master_monitor = otherWorkersMonitorMap[next_master];
//     // masterMonitor = next_master_monitor;
//     // otherWorkersMonitorMap.erase(next_master);
//     // std::cout << "P" << id << ": Monitoring these remaining workers: ";
//     // for ( auto& x: otherWorkersMonitorMap )
//     //   std::cout << "\t" << x.first << ": " << x.second << std::endl;
//
//     return false;
//
//   }
//   else
//     return false;
// }
