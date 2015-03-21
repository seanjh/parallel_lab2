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
#include "MW_Random.hpp"

// #define CHECKPOINT_PERIOD 1.0
// const double CHECKPOINT_PERIOD = 1.0;
const std::string WORK_CHECKPOINT_FILENAME    = "checkpoint_work.csv";
const std::string RESULTS_CHECKPOINT_FILENAME = "checkpoint_result.csv";

MW_Master::MW_Master(int myid, int sz, const std::list<std::shared_ptr<Work>> &work_p) :
  id(myid), world_size(sz)
{

  MW_ID nextWorkID = 0;
  for(auto iter = work_p.begin();iter != work_p.end(); iter++)
    work[nextWorkID++] = *iter;

  workToDo = work;
  //std::cout << "Total work in master is " << workToDo->size() << std::endl;

  initializeWorkerMap();

  performCheckpoint();
  sendHeartbeat();
}

void MW_Master::initializeWorkerMap()
{
  for (int i=0; i<world_size; i++) {
    if (i != id) {
      // workers->push_back(i);
      workerMap[i] = std::shared_ptr<MW_Remote_Worker> (new MW_Remote_Worker(i));
      // workerMap[i] = MW_Remote_Worker(i);
    }
  }
}

std::shared_ptr<MW_Master> MW_Master::restore(int myid, int size)
{
  return std::make_shared<MW_Master> (myid, size);
}


std::shared_ptr<std::list<std::shared_ptr<Result>>> MW_Master::getResults()
{
  //create new list to return
  std::shared_ptr<std::list<std::shared_ptr<Result>>> retList (new std::list<std::shared_ptr<Result>>);

  for(auto iter = results.begin();iter != results.end(); iter++)
    retList->push_back(iter->second);

  return retList;
}

void MW_Master::master_loop()
{
  MW_Random random = MW_Random(id, world_size);
  int worker_id;
  long long int iteration_count=0;

  while (1) {
    //std::cout << "P:" << id << " Master Loop\n";
    // if (random.random_fail()) {
    //   std::cout << "P:" << id << " MASTER FAILURE EVENT\n";
    //   MPI::Finalize();
    //   exit (0);
    // }

    checkOnWorkers();

    // if(shouldSendHeartbeat()) sendHeartbeat();
    // else
    if (shouldCheckpoint()) performCheckpoint();
    else if (hasWorkersHasWork()) {

      std::cout << "MASTER IS SENDING\n";
      worker_id = nextWorker();
      send(worker_id);

    } else if (noWorkersHasWork() || noWorkersNoWork()) {

      //std::cout << "MASTER IS WAITING FOR A RESULT\n";
      receive();

    } else if (hasWorkersNoWork()) {
      std::cout << "No Work!!\n";
      if (hasAllWorkers()) {
        std::cout << "MASTER IS DONE\n";
        send_done();

        break;
      } else {
        receive();
      }
    } else {
      std::cout << "WTF happened here\n";
      assert(0);
    }
  }
}

void MW_Master::checkOnWorkers()
{
  // TODO: mark dead workers
}

void MW_Master::send_done()
{
  // TODO: only send to alive workers
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
  // std::cout << "P:" << this->id << " sending work to process " << worker_id << std::endl;

  // for(auto it=workToDo.begin(); it!=workToDo.end(); it++)
  //     std::cout << it->first <<": "<<*(it->second->serialize())<<std::endl;

  auto workPair = *(workToDo.begin());
  MW_ID workID = workPair.first;
  workToDo.erase(workID);
  std::shared_ptr<Work> work = workPair.second;

  std::shared_ptr<MW_Remote_Worker> rm = workerMap[worker_id];
  rm->markPending(workID);

  std::shared_ptr<std::string> work_string = std::make_shared<std::string> (std::to_string(workPair.first) + "," + *(work->serialize()));

  int count = (int) work_string->length();

  //Need to add ID to message

  // std::cout << "P:" << id << " sending work with " << count <<
  //   " total MPI::CHARs -- " << *work_string << std::endl;

  MPI::COMM_WORLD.Send(
    (void *) work_string->data(),
    count,
    MPI::CHAR,
    worker_id,
    WORK_TAG
  );

  // std::cout << "P:" << id << " finished send to P" << worker_id << ". " <<
  //   workToDo->size() << " work items remaining" << std::endl;

  // delete work;
  // delete work_string;

}

void MW_Master::receive()
{
  // std::cout << "P:" << id << " master waiting to receive" << std::endl;
  MPI::Status status;
  bool can_receive = MPI::COMM_WORLD.Iprobe(
    MPI::ANY_SOURCE,
    MPI::ANY_TAG,
    status
  );

  if (!can_receive) {
    //std::cout << "IProbe did not find a message. No receive possible\n";
    return;
  }

  // std::string& message = new std::string(1000, 0);
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
      process_heartbeat(worker_id);
      break;
    }

    case CHECKPOINT_DONE_TAG: {
      process_checkpoint_done(worker_id);
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
    // std::cout << "P:" << id << " Received from process " << worker_id <<
    //   " message of length "<< count << " \"" << serializedObject << "\"\n";

    // std::cout << "App is: " << app << std::endl;
    // Result *result = app->resultDeserializer(serializedObject);
    std::istringstream iss (messageString);
    std::string idString, serializedObject;

    std::getline(iss,idString,',');
    // std::cout<<idString<<std::endl;
    MW_ID result_id = std::stoul(idString);
    std::getline(iss,serializedObject);
    // std::cout<<serializedObject<<std::endl;

    // MPIMessage *mpi_message = new MPIMessage(serializedObject);
    auto mpi_message = std::make_shared<MPIMessage>(serializedObject);
    // std::cout << "P:" << id << " mpi_message (result) is " << mpi_message->to_string() << std::endl;
    std::shared_ptr<Result> result = mpi_message->deserializeResult();
    // delete mpi_message;

    // std::cout << "P:" << id << " received results (" << result << ") from process " << worker_id << ". " << std::endl;
    assert(result != NULL);

    std::shared_ptr<MW_Remote_Worker> rm = workerMap[worker_id];
    rm->markCompleted(result_id);
    results[result_id] = result;
    //results->push_back(result);
    // std::cout << "results size is " << results.size() << std::endl;

    // for(auto it=results.begin(); it!=results.end(); it++)
    //   std::cout << it->first <<": "<<*(it->second->serialize())<<std::endl;
  }

  // Reinclude this worker in the queue
  // workers->push_back(worker_id);

}

void MW_Master::process_heartbeat(int worker_id)
{
  auto worker = workerMap[worker_id];
  worker->heartbeatMonitor.addHeartbeat();
}

void MW_Master::process_checkpoint_done(int worker_id)
{
  // TODO: implement
  return;
}

bool MW_Master::shouldCheckpoint()
{
  //return count % CHECKPOINT_COUNT == 0;
  return (MPI::Wtime() - lastCheckpoint) > CHECKPOINT_PERIOD;
}
void MW_Master::performCheckpoint()
{
  std::cout <<"Starting Checkpoint" << std::endl;

  std::ofstream checkpointWorkFile, checkpointResultsFile;
  checkpointWorkFile.open(WORK_CHECKPOINT_FILENAME);

  for(auto it=work.begin(); it!=work.end(); it++)
      checkpointWorkFile << it->first <<","<<*(it->second->serialize())<<std::endl;

  checkpointWorkFile.close();

  checkpointResultsFile.open(RESULTS_CHECKPOINT_FILENAME);
  for(auto it=results.begin(); it!=results.end(); it++)
      checkpointResultsFile << it->first <<","<<*(it->second->serialize())<<std::endl;

  checkpointResultsFile.close();

  lastCheckpoint = MPI::Wtime();
  std::cout <<"Completed Checkpoint" << std::endl;
}

MW_Master::~MW_Master() { }

bool MW_Master::shouldSendHeartbeat()
{
  return (MPI::Wtime() - lastHeartbeat) > HEARTBEAT_PERIOD;
}

void MW_Master::sendHeartbeat()
{
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->heartbeatMonitor.isAlive())
      it->second->heartbeatMonitor.sendHeartbeat(true);
  }
  lastHeartbeat = MPI::Wtime();
}

bool MW_Master::hasWorkersHasWork()
{
  return (nextWorker() != -1) && !workToDo.empty();
}

bool MW_Master::hasWorkersNoWork()
{
  return (nextWorker() != -1) && workToDo.empty();
}

bool MW_Master::noWorkersHasWork()
{
  return (nextWorker() == -1) && !workToDo.empty();
}

bool MW_Master::noWorkersNoWork()
{
  return (nextWorker() == -1) && workToDo.empty();
}

bool MW_Master::hasAllWorkers()
{
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->heartbeatMonitor.isAlive() && !it->second->isAvailable())
      return false;
  }
  return true;
  //return workers->size() == world_size - 1;  // exclude self (master)
}


int MW_Master::nextWorker()
{
  for(auto it=workerMap.begin(); it != workerMap.end(); it++)
  {
    if (it->second->isAvailable())
      return it->first;
  }
  //assert(false;)

  return -1;
}

void MW_Master::initializeResultFromCheckpoint()
{
  std::cout << "P" << id << ": RESTORING RESULTS\n";
  std::string line, idString, serializedObject;
  MW_ID id;
  std::shared_ptr<MPIMessage> message;
  std::ifstream infile (RESULTS_CHECKPOINT_FILENAME);

  if (infile.is_open())
  {
    while (getline (infile, line))
    {
      std::cout << "line: " << line << '\n';

      std::istringstream iss (line);
      std::getline(iss, idString,',');
      id = std::stoul(idString);
      std::cout << "idString: " << idString << " (MW_ID=" << id << ")" << '\n';
      std::getline(iss, serializedObject);
      std::cout << "serializedObject: " << serializedObject << '\n';

      message = std::make_shared<MPIMessage> (serializedObject);
      std::cout << "MPIMessage: " << message->to_string() << '\n';
      results[id] = message->deserializeResult();
    }
    infile.close();
  }
  else std::cerr << "Unable to open file";
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
      std::cout << "line: " << line << '\n';

      std::istringstream iss (line);
      std::getline(iss, idString,',');
      id = std::stoul(idString);
      std::cout << "idString: " << idString << " (MW_ID=" << id << ")" << '\n';
      std::getline(iss, serializedObject);
      std::cout << "serializedObject: " << serializedObject << '\n';

      message = std::make_shared<MPIMessage> (serializedObject);
      std::cout << "MPIMessage: " << message->to_string() << '\n';
      work[id] = message->deserializeWork();
    }
    infile.close();
  }
  else std::cerr << "Unable to open file";
}


MW_Master::MW_Master(int myid, int size) : id(myid), world_size(size)
{
  std::cout << "P" << myid << ": Creating NEW Master from checkpoint\n";

  initializeWorkerMap();

  initializeWorkFromCheckpoint();

  initializeResultFromCheckpoint();

  std::cout << "workToDo is size " << workToDo.size() << "\n";
  std::shared_ptr<Result> result;
  for ( auto it = work.begin(); it != work.end(); ++it )
  {
    try {
      result = results.at(it->first);
    }
    catch (const std::out_of_range& oor) {
      std::cerr << "WORK is not in RESULTS. Out of Range error: " << oor.what() << '\n';
      workToDo[it->first] = it->second;
      std::cout << "workToDo is size " << workToDo.size() << "\n";
    }
  }

}
