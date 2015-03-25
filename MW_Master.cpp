#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <string>
#include <mpi.h>

#include "MW_Master.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MPIMessage.hpp"

const std::string WORK_CHECKPOINT_FILENAME    = "checkpoint_work.csv";
const std::string RESULTS_CHECKPOINT_FILENAME = "checkpoint_result.csv";


MW_Master::MW_Master(int myid, int sz, const std::list<std::shared_ptr<Work>> &work_p) :
  id(myid), world_size(sz), random(MW_Random(MASTER_FAILURE_PROBABILITY, myid, sz))
  // id(myid), world_size(sz)
{

  MW_ID nextWorkID = 0;
  for(auto iter = work_p.begin();iter != work_p.end(); iter++)
    work[nextWorkID++] = *iter;

  workToDo = work;
  std::cout << "P" << id << ": Total work in master is " << workToDo.size() << std::endl;

  delayUntil = MPI::Wtime() + STARTUP_DELAY_TIME;

  performCheckpoint();
  broadcastHeartbeat();
}

MW_Master::MW_Master(int myid, int size) : id(myid), world_size(size),
  random(MW_Random(MASTER_FAILURE_PROBABILITY, myid, size))
// MW_Master::MW_Master(int myid, int size) : id(myid), world_size(size)
{
  std::cout << "P" << myid << ": Creating NEW Master from checkpoint\n";

  broadcastHeartbeat();

  initializeWorkFromCheckpoint();
  initializeResultFromCheckpoint();
  initializeWorkToDoFromCheckpoint();

  std::cout << "P" << id << ": Completed RESTORE (work: " <<
    work.size() << ", results: " << results.size() << ")" <<
    work.size() << ", workToDo: " << workToDo.size() << ")\n";

  // std::cout << "P" << id << ": Restored " << work.size() << " total work\n";
  // std::cout << "P" << id << ": Restored " << results.size() << " total results\n";
  // std::cout << "P" << myid << ": " << workToDo.size() << " remaining units of work TODO.\n";

  lastCheckpoint = MPI::Wtime();

  broadcastHeartbeat();



  broadcastHeartbeat();

  delayUntil = MPI::Wtime() + STARTUP_DELAY_TIME;

}

MW_Master::~MW_Master() { }

std::shared_ptr<MW_Master> MW_Master::restore(int myid, int size)
{
  return std::make_shared<MW_Master> (myid, size);
}


std::shared_ptr<std::list<std::shared_ptr<Result>>> MW_Master::getResults()
{
  // Create new list to return
  std::shared_ptr<std::list<std::shared_ptr<Result>>> retList (new std::list<std::shared_ptr<Result>>);

  for(auto iter = results.begin();iter != results.end(); iter++)
    retList->push_back(iter->second);

  return retList;
}

void MW_Master::keepInTouch()
{
  if(shouldSendHeartbeat())
    sendHeartbeat();
  receiveHeartbeat();
}

bool MW_Master::master_loop()
{
  bool done = false;
  int worker_id;
  long long int iteration_count=0;

  // std::cout<<"P" << id <<": entering startup loop"<<std::endl;
  // Master receives heartbeats and send heartbeats with workers
  while (delayUntil > MPI::Wtime())
  {
    keepInTouch();
    // if(shouldSendHeartbeat())
    //   sendHeartbeat();
    //
    // receive();
  }

  // Then tell everyone we're Master
  broadcastNewMasterSignal();

  // std::cout<<"P" <<id<<": entering main Master loop"<<std::endl;
  while (1) {

    checkOnWorkers();

    if(shouldSendHeartbeat()) {

      sendHeartbeat();

    } else if (shouldCheckpoint()) {

      performCheckpoint();

    } else if (hasWorkers() && hasWorkToDistribute()) {

      // std::cout << "MASTER IS SENDING WORK\n";
      worker_id = nextWorker();
      send(worker_id);

    } else if (hasPendingWork()) {

      // std::cout << "MASTER IS WAITING FOR A RESULT\n";
      receive();

    } else if (!hasPendingWork()) {

      send_done();
      done = true;
      break;

    } else {
      std::cout << "WTF happened here\n";
      assert(0);
    }
  }

  return done;
}

void MW_Master::checkOnWorkers()
{
  bool foundAWorker = false;
  bool workerAlive;
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->heartbeatMonitor.isAlive())
      foundAWorker = true;

    workerAlive = it->second->heartbeatMonitor.isAlive();
    if (!workerAlive) {
      // std::cout << "P" << id << ": Worker (P" << it->second->id << ")  looks DEAD" << std::endl;

      if (it->second->workPendingCount() > 0) {
        // Move dead Worker's pending work back on workToDo queue
        std::cout << "P" << id << ": Worker (P" << it->second->id << ")  looks DEAD" << std::endl;
        std::list<MW_ID> &pendingWorkList = it->second->getPendingWork();
        while(!pendingWorkList.empty())
        {
          auto work_id = pendingWorkList.front();
          std::cout << "P" << id << ": Moving " << work_id << " back on to work list" <<std::endl;
          pendingWorkList.pop_front();
          workToDo[work_id] = work[work_id];
        }
      }

    }
  }

  if(!foundAWorker)
  {
    std::cout << "P" << id << ": THERE ARE NO MORE WORKERS!\n";
    MPI::Finalize();
    exit (1);
  }

}

void MW_Master::send_done()
{
  //std::cout << "MASTER SENDING DONE MESSAGES\n";
  for (int process_id=0; process_id<world_size; process_id++) {
    if (process_id != id) {
      //std::cout << "MASTER SENDING DONE to PROCESS" << process_id << " \n";
      MPI::COMM_WORLD.Send(
        0,
        0,
        MPI::CHAR,
        process_id,
        DONE_TAG
      );
    }
  }
}


void MW_Master::send(int worker_id)
{
  assert(!workToDo.empty());

  auto workPair = *(workToDo.begin());
  MW_ID workID = workPair.first;
  workToDo.erase(workID);
  std::shared_ptr<Work> work = workPair.second;
  std::shared_ptr<MW_Remote_Worker> worker_remote = workerMap[worker_id];
  worker_remote->markPending(workID);

  std::shared_ptr<std::string> work_string = std::make_shared<std::string> (std::to_string(workPair.first) + "," + *(work->serialize()));

  int count = (int) work_string->length();

  MPI::COMM_WORLD.Send(
    (void *) work_string->data(),
    count,
    MPI::CHAR,
    worker_id,
    WORK_TAG
  );

}

void MW_Master::receiveHeartbeat() {
  MPI::Status status;
  bool can_receive;
  can_receive = MPI::COMM_WORLD.Iprobe(
    MPI::ANY_SOURCE,
    HEARTBEAT_TAG,
    status
  );

  if (!can_receive) {
    // std::cout << "P" << id << ": IProbe did not find heartbeat message. No receive possible\n";
    return;
  }

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);

  int worker_id, count;

  MPI::COMM_WORLD.Recv(
    (void *) message,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    MPI::ANY_SOURCE,
    HEARTBEAT_TAG,
    status
  );

  worker_id = status.Get_source();
  count = status.Get_count(MPI::CHAR);

  process_heartbeat(worker_id);
}

void MW_Master::receive()
{
  // std::cout << "P" << id << ": Master is waiting to receive" << std::endl;
  MPI::Status status;
  bool can_receive = MPI::COMM_WORLD.Iprobe(
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    status
  );

  if (!can_receive) {
    // std::cout << "P" << id << ": IProbe did not find a message. No receive possible\n";
    return;
  }

  char *message = (char*)malloc(MAX_MESSAGE_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE);

  MPI::COMM_WORLD.Recv(
    (void *) message,
    MAX_MESSAGE_SIZE,
    MPI::CHAR,
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    status
  );

  MWTag mw_tag = static_cast<MWTag>(status.Get_tag());
  int worker_id = status.Get_source();
  int count = status.Get_count(MPI::CHAR);

  switch (mw_tag)
  {
    case WORK_TAG: {
      process_result(worker_id, count, message);
      break;
    }

    case HEARTBEAT_TAG: {
      // std::cout<<"Master Recieved heartbeat" << std::endl;
      process_heartbeat(worker_id);
      break;
    }

    default: {
      std::cout << "WTF Happened here\n";
      break;
    }
  }

  free(message);
}

void MW_Master::process_result(int worker_id, int count, char *message)
{
  if (count != 0) {
    std::string messageString = std::string(message, count);

    std::istringstream iss (messageString);
    std::string idString, serializedObject;

    std::getline(iss,idString,',');
    MW_ID result_id = std::stoul(idString);
    std::getline(iss,serializedObject);

    auto mpi_message = std::make_shared<MPIMessage>(serializedObject);
    std::shared_ptr<Result> result = mpi_message->deserializeResult();

    // std::cout << "P" << id << ": received results (" << result << ") from process " << worker_id << ". " << std::endl;
    assert(result != NULL);

    std::shared_ptr<MW_Remote_Worker> worker_remote = workerMap[worker_id];
    worker_remote->markCompleted(result_id);
    results[result_id] = result;
    completedWork[result_id] = work[result_id];
    // std::cout << "Added to completed work list" <<std::endl;

  }
}

void MW_Master::process_heartbeat(int worker_id)
{
  // std::cout << "P" << id << ": Received heartbeat from " << worker_id << std::endl;
  std::shared_ptr<MW_Remote_Worker> worker;
  try {
    worker = workerMap.at(worker_id);
  }
  catch (const std::out_of_range& oor) {
    // std::cout << "P" << id << ": Received its first heartbeat from " << worker_id << ". Adding to monitor map\n";
    worker = std::shared_ptr<MW_Remote_Worker> (new MW_Remote_Worker(worker_id));
    workerMap[worker_id] = worker;
  }
  worker->heartbeatMonitor.addHeartbeat();
}

bool MW_Master::shouldCheckpoint()
{
  return (MPI::Wtime() - lastCheckpoint) > CHECKPOINT_PERIOD;
}

void MW_Master::performCheckpoint()
{
  std::cout << "P" << id << ": Starting Checkpoint" << std::endl;

  std::ofstream checkpointWorkFile, checkpointResultsFile;
  checkpointWorkFile.open(WORK_CHECKPOINT_FILENAME);

  for(auto it=work.begin(); it!=work.end(); it++) {
    checkpointWorkFile << it->first <<","<<*(it->second->serialize())<<std::endl;

    keepInTouch();
  }

  checkpointWorkFile.close();

  checkpointResultsFile.open(RESULTS_CHECKPOINT_FILENAME);
  for(auto it=results.begin(); it!=results.end(); it++) {
    checkpointResultsFile << it->first <<","<<*(it->second->serialize())<<std::endl;

    keepInTouch();
  }

  checkpointResultsFile.close();

  lastCheckpoint = MPI::Wtime();
  std::cout << "P" << id << ": Completed Checkpoint (work: " << work.size() << ", results: " << results.size() << ")" << std::endl;
}

bool MW_Master::shouldSendHeartbeat()
{
  return (MPI::Wtime() - lastHeartbeat) > HEARTBEAT_PERIOD;
}

void MW_Master::sendHeartbeat()
{
  if (random.random_fail()) {
    std::cout << "P" << id << ": MASTER FAILURE EVENT\n";
    MPI::Finalize();
    exit (0);
  }

  broadcastHeartbeat();
}

bool MW_Master::hasWorkToDistribute()
{
  return !workToDo.empty();
}

void MW_Master::printWorkStatus()
{
  int work_results_diff = work.size() - results.size();
  std::cout << "P" << id << ": work " << work.size() <<
    ", workToDo " << workToDo.size() <<
    ", completedWork " << completedWork.size() <<
    ", results " << results.size() <<
    ", work-results " << work_results_diff <<
    "\n";
}

bool MW_Master::hasPendingWork()
{
  bool pendingWork = !(completedWork.size() == work.size());

  if (!pendingWork){
    std::cout << "P" << id << ": there is no more pending work" << std::endl;
    // printWorkStatus();
    if (!(work.size() == results.size()))
      printWorkStatus();
    assert(work.size() == results.size());

  }
  else {
    if (!(work.size() != results.size()))
      printWorkStatus();
    assert(work.size() != results.size());
  }

  return pendingWork;
}

bool MW_Master::hasWorkers()
{
  return (nextWorker() != -1);
}

bool MW_Master::hasAllWorkers()
{
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->heartbeatMonitor.isAlive() && !it->second->isAvailable())
      return false;
  }
  return true;
}


int MW_Master::nextWorker()
{
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->isAvailable())
      return it->first;
  }

  return -1;
}

void MW_Master::initializeResultFromCheckpoint()
{
  std::cout << "P" << id << ": RESTORING RESULTS\n";
  std::string line, idString, serializedObject;
  MW_ID work_id;
  std::shared_ptr<MPIMessage> message;
  std::ifstream infile (RESULTS_CHECKPOINT_FILENAME);

  if (infile.is_open())
  {
    while (getline (infile, line))
    {
      std::istringstream iss (line);
      std::getline(iss, idString,',');
      work_id = std::stoul(idString);
      std::getline(iss, serializedObject);

      message = std::make_shared<MPIMessage> (serializedObject);
      results[work_id] = message->deserializeResult();
      completedWork[work_id] = work[work_id];

      keepInTouch();
    }
    infile.close();

    std::cout << "Restored " << results.size() << " results." <<std::endl;
  }
  else {
    std::cerr << "Unable to open file";
  }
}

void MW_Master::initializeWorkFromCheckpoint()
{
  std::cout << "P" << id << ": RESTORING WORK\n";
  std::string line, idString, serializedObject;
  MW_ID id;
  std::shared_ptr<MPIMessage> message;
  std::ifstream infile (WORK_CHECKPOINT_FILENAME);

  if (infile.is_open())
  {
    while (getline (infile, line))
    {

      std::istringstream iss (line);
      std::getline(iss, idString,',');
      id = std::stoul(idString);
      std::getline(iss, serializedObject);

      message = std::make_shared<MPIMessage> (serializedObject);
      work[id] = message->deserializeWork();

      keepInTouch();
    }
    infile.close();
  }
  else std::cerr << "Unable to open file";
}

void MW_Master::initializeWorkToDoFromCheckpoint()
{
  std::cout << "workToDo is size " << workToDo.size() << "\n";
  std::shared_ptr<Result> result;
  for ( auto it = work.begin(); it != work.end(); ++it )
  {
    try {
      result = results.at(it->first);
    }
    catch (const std::out_of_range& oor) {
      // Work is not in Results. It belongs in workToDo
      workToDo[it->first] = it->second;
    }

    keepInTouch();
  }

}

void MW_Master::broadcastNewMasterSignal()
{
  std::cout << "P" << id << ": Broadcasting to Workers I AM MASTER\n";
  for (int i=0; i<world_size; i++)
  {
    if (i != id)
    {
      MPI::COMM_WORLD.Send(
        0,
        0,
        MPI::CHAR,
        i,
        NEW_MASTER_TAG
      );
    }
  }
  // std::cout << "P" << id << ": Finished broadcast.\n";
}

void MW_Master::broadcastHeartbeat()
{
  // std::cout << "P" << id << ": Master Broadcasting heartbeat" << std::endl;
  for (int i=0; i<world_size; i++)
  {
    if (i != id)
    {
      // std::cout << "P" << id << ": Sending heartbeat to " << i <<std::endl;
      char m = 1;
      MPI::COMM_WORLD.Send(
        (void *) &m,
        1,
        MPI::CHAR,
        i,
        HEARTBEAT_TAG
      );
      // std::cout << "P" << id << ": Sent heartbeat to " << id <<std::endl;
    }
  }
  // std::cout << "P" << id << " Master done sending heartbeats"<<std::endl;
  lastHeartbeat = MPI::Wtime();
}
