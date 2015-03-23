#ifndef __MW__MASTER__
#define __MW__MASTER__

#include <memory>
#include <list>
#include <unordered_map>

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MPIMessage.hpp"
#include "MW_Monitor.hpp"
#include "MW_Remote_Worker.hpp"
#include "MW_Random.hpp"
#include "MW_Process.hpp"

extern const double       CHECKPOINT_PERIOD;
extern const std::string  WORK_CHECKPOINT_FILENAME;
extern const std::string  RESULTS_CHECKPOINT_FILENAME;

class MW_Master : public MW_Process {
public:
  MW_Master(int, int, const std::list<std::shared_ptr<Work>> &);
  MW_Master(int, int); // Restores from checkpoint
  ~MW_Master();
  static std::shared_ptr<MW_Master> restore(int, int);
  bool master_loop();
  virtual bool isMaster() {return true;};
  std::shared_ptr<std::list<std::shared_ptr<Result>>> getResults();

private:
  int id;
  int world_size;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> work;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> workToDo;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> completedWork;
  std::unordered_map<MW_ID, std::shared_ptr<Result>> results;
  std::unordered_map<int, std::shared_ptr<MW_Remote_Worker>> workerMap;
  // std::list<int> freeWorkers;

  double lastCheckpoint;
  double lastHeartbeat;
  bool willFail;
  MW_Random random;

  void initializeWorkerMap();
  void initializeRandomFailure();
  void initializeWorkFromCheckpoint();
  void initializeResultFromCheckpoint();
  int nextWorker();
  void checkOnWorkers();
  void send_done();
  void send(int);
  void receive();
  void process_result(int, int, char *);
  void process_heartbeat(int);
  bool hasAllWorkers();
  void performCheckpoint();
  bool shouldCheckpoint();
  bool shouldSendHeartbeat();
  void sendHeartbeat();
  void broadcastHeartbeat();
  void broadcastNewMasterSignal();
  bool hasWorkers();
  bool hasPendingWork();
  void printWorkStatus();
  bool hasWorkToDistribute();

};

#endif /* defined(__MW__MASTER__) */
