#include <assert.h>
#include <iostream>
#include <sstream>
#include <mpi.h>

#include "MW_Worker.hpp"
#include "MPIMessage.hpp"

MW_Worker::MW_Worker(const int myid, const int m_id, const int w_size) :
  id(myid), world_size(w_size), preemptionTimer(.1),
  random(MW_Random(WORKER_FAILURE_PROBABILITY, myid, w_size))
{
  sendHeartbeat();
}

MW_Worker::~MW_Worker() { }

MWTag MW_Worker::receiveWork()
{

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  int source_id=-1;
  int count=0;

  MWTag message_tag = receive(MPI::ANY_SOURCE,
                              WORK_TAG,
                              (void*)message,
                              MAX_MESSAGE_SIZE,
                              source_id,
                              count);
  if(message_tag == WORK_TAG)
  {
    process_work(message, source_id, count);
  }
  else
  {
    assert(message_tag == NOTHING_TAG);
  }

  free(message);
  return message_tag;
}

MWTag MW_Worker::receiveHeartbeat()
{

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  int source_id=-1;
  int count=0;

  MWTag message_tag = receive(MPI::ANY_SOURCE,
                              HEARTBEAT_TAG,
                              (void*)message,
                              MAX_MESSAGE_SIZE,
                              source_id,
                              count);
  if(message_tag == HEARTBEAT_TAG)
  {
    // std::cout << "P" << id<< ": Received heartbeat from "<<source_id<<std::endl;
    process_heartbeat(source_id);
  }
  else
  {
    assert(message_tag == NOTHING_TAG);
  }

  free(message);
  return message_tag;
}

MWTag MW_Worker::receiveNewMaster(int &newMaster)
{

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  int source_id=-1;
  int count=0;

  MWTag message_tag = receive(MPI::ANY_SOURCE,
                              NEW_MASTER_TAG,
                              (void*)message,
                              MAX_MESSAGE_SIZE,
                              source_id,
                              count);
  if(message_tag == NEW_MASTER_TAG)
  {
    std::cout<< "P" << id <<" :Received new master from " << source_id <<std::endl;
    newMaster = source_id;
  }
  else
  {
    assert(message_tag == NOTHING_TAG);
  }

  free(message);
  return message_tag;
}

MWTag MW_Worker::receiveDone()
{

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);
  int source_id=-1;
  int count=0;

  MWTag message_tag = receive(MPI::ANY_SOURCE,
                              DONE_TAG,
                              (void*)message,
                              MAX_MESSAGE_SIZE,
                              source_id,
                              count);

  free(message);
  return message_tag;
}


MWTag MW_Worker::receive(int source, int tag, void *message, int maxMessageSize,
                         int &source_id, int &count)
{
  MPI::Status status;
  bool can_receive = MPI::COMM_WORLD.Iprobe(
    source,
    tag,
    status
  );

  if (can_receive) {

    MPI::COMM_WORLD.Recv(
      message,
      maxMessageSize,
      MPI::CHAR,
      source,
      tag,
      status
    );

    MWTag message_tag = (MWTag)status.Get_tag();
    source_id = status.Get_source();
    count = status.Get_count(MPI::CHAR);

    return message_tag;

  }

  return NOTHING_TAG;

}

void MW_Worker::process_work(char *message, int source_id, int count)
{
  // assert(masterMonitor);
  assert(source_id == master_id);
  std::string messageString = std::string(message, count);
  // std::cout << "P" << id << ": Received from master P" << master_id << " message \"" << messageString << "\"\n";

  std::istringstream iss (messageString);
  std::string idString, serializedObject;

  // Pull off the prepended work MW_ID
  std::getline(iss,idString,',');
  MW_ID work_id = std::stoul(idString);
  // Pull off the work message
  std::getline(iss,serializedObject);
  auto mpi_message = std::make_shared<MPIMessage>(serializedObject);

  std::shared_ptr<Work> work = mpi_message->deserializeWork();
  assert(work != NULL);
  workToDo[work_id] = work;
}

void MW_Worker::process_heartbeat(int source_id)
{
  std::shared_ptr<MW_Monitor> nodeMonitor;
  try {
    nodeMonitor = otherWorkersMonitorMap.at(source_id);
  }
  catch (const std::out_of_range& oor) {

    // std::cout << "P" << id << ": Received its first heartbeat from " << source_id << ". Adding to monitor map\n";
    nodeMonitor = std::shared_ptr<MW_Monitor>(new MW_Monitor(source_id, HEARTBEAT_PERIOD*5.0));
    otherWorkersMonitorMap[source_id] = nodeMonitor;
  }
  nodeMonitor->addHeartbeat();
}


void MW_Worker::send(MW_ID result_id, std::shared_ptr<Result> result)
{
  std::shared_ptr<std::string> result_string = (
    std::make_shared<std::string> (std::to_string(result_id) +
    "," + *(result->serialize()))
  );

  int count = (int) result_string->length();
  MPI::COMM_WORLD.Send(
    (void *) result_string->data(),
    count,
    MPI::CHAR,
    master_id,
    WORK_TAG
  );
}


bool MW_Worker::worker_loop()
{
  MWTag message_tag;
  bool done = false;

  // std::cout << "P" << id << ": starting initial delay" <<std::endl;
  // Wait a little while for Master to start communicating
  double delayTime = MPI::Wtime() + STARTUP_DELAY_TIME;
  while(delayTime > MPI::Wtime())
  {
    if(shouldSendHeartbeat())
    {
      // std::cout << "P"<<id << " worker sending heartbeat from delay loop" <<std::endl;
      sendHeartbeat();
      continue;
    }
    receiveHeartbeat();
  }


  // std::cout << "P"<< id << ": Starting to wait for master" <<std::endl;
  // Wait for Master loop
  while(1)
  {
    int newMasterId = -1;
    MWTag masterTag = receiveNewMaster(newMasterId);
    if(masterTag == NEW_MASTER_TAG)
    {

      int projectedMaster = findNextMasterId();
      // std::cout << "P"<<id <<": Waiting for master "<<projectedMaster << " received " << newMasterId << std::endl;
      assert(newMasterId == projectedMaster);
      master_id = projectedMaster;
      nextMasterCheckTime = MPI::Wtime();
      masterMonitor = getMasterMonitor();
      break;
    }

    if(shouldSendHeartbeat())
    {
      // std::cout << "P" << id << ": worker sending heartbeat from master waiting loop" <<std::endl;
      int projectedMaster = findNextMasterId();
      // std::cout << "P" << id <<": Waiting for master "<<projectedMaster<<std::endl;

      // While I was waiting for Master, Master died
      // I should become the next Master
      if(id == projectedMaster)
      {
        std::cout << "P"<<id <<": Waited for new Master, but it looks dead. I should be the next Master" << std::endl;
        return false;
      }

      sendHeartbeat();
      continue;
    }

    receiveHeartbeat();
  } // END Wait for Master loop



  // std::cout << "P"<<id<< ": Starting main Work loop" <<std::endl;
  // Main worker loop
  while (1) {

    if(shouldSendHeartbeat())
    {
      // std::cout << "P"<<id << ": worker sending heartbeat from main loop" <<std::endl;
      sendHeartbeat();
      continue;
    }

    if (shouldCheckOnMaster())
    {
      // std::cout << "P" << id << ": Checking on Master\n";
      updateMasterCheckTime();

      bool is_master_alive = isMasterAlive();
      if (is_master_alive) {
        // std::cout << "P" << id << ": Master "<< master_id << " looks OK!\n";
        continue;
      } else {
        std::cout << "P" << id << ": MASTER "<< master_id << " LOOKS DEAD\n";
        std::cout << "P" << id << ": Current TIme " << MPI::Wtime() << std::endl;
        // masterMonitor->dump();
        // assert(false);
        //add delay

        return false; // NOT DONE
      }
    }

    // RECEIVE MESSAGES
    if(receiveHeartbeat() != NOTHING_TAG)
      continue;

    if(receiveWork() != NOTHING_TAG)
      continue;

    // PROCESS WORK
    if (hasWork()) {
      auto workPair = *(workToDo.begin());
      MW_ID work_id = workPair.first;

      std::shared_ptr<Work> work = workPair.second;

      MW_API_STATUS_CODE status;

      preemptionTimer.reset();
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

        continue;

      }
      else
      {
        // std::cout <<"Worker returning preempted"<<std::endl;
        continue;
      }
    }
    else
    {
      if(receiveDone() != NOTHING_TAG)
        return true;
    }

  } // END Main worker loop

  return done;
}

bool MW_Worker::shouldSendHeartbeat()
{
  return (MPI::Wtime() - lastHeartbeat) > HEARTBEAT_PERIOD;
}

void MW_Worker::sendHeartbeat()
{

  if (random.random_fail()) {
    std::cout << "P" << id << ": WORKER FAILURE EVENT\n";
    MPI::Finalize();
    exit (0);
  }
  // std::cout << "P" << id << " worker sending heartbeat" <<std::endl;

  broadcastHeartbeat();

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

bool MW_Worker::shouldCheckOnMaster()
{
  return MPI::Wtime() > nextMasterCheckTime;
}

int MW_Worker::findNextMasterId()
{
  int lowest_other_worker_id = world_size - 1;
  for ( auto& it: otherWorkersMonitorMap ) {
    if (it.second->isAlive()) {
      // std::cout << "\tP" << id << ": I think P" << it.first << " is ALIVE.\n";
      if (it.first < lowest_other_worker_id) {
        lowest_other_worker_id = it.first;
      }
    } else {
      // std::cout << "\tP" << id << ": I think P" << it.first << " is DEAD.\n";
    }
  }

  return id < lowest_other_worker_id ? id : lowest_other_worker_id;
}

void MW_Worker::updateMasterCheckTime()
{
  nextMasterCheckTime += (HEARTBEAT_PERIOD*2.0);
}

bool MW_Worker::isMasterAlive()
{
  assert(masterMonitor);

  if(masterMonitor->isAlive()) {
    return true;
  }
  return false;
}

std::shared_ptr<MW_Monitor> MW_Worker::getMasterMonitor()
{
  int masterId = findNextMasterId();
  assert(masterId != id);
  return otherWorkersMonitorMap[masterId];
}


bool MW_Worker::transitionMaster()
{
  std::cout << "P" << id << ": Transition to next Master beginning\n";
  int nextMasterId = findNextMasterId();
  bool is_next_master = (nextMasterId == id);

  if (is_next_master) {

    std::cout << "P" << id << ": I AM THE NEXT MASTER\n";

  } else {

    std::cout << "P" << id << ": P" << nextMasterId << " looks like the next MASTER\n";

  }

  return is_next_master;
}
