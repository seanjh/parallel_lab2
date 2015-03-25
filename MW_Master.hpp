#ifndef __MW__MASTER__
#define __MW__MASTER__

#include <memory>
#include <list>
#include <unordered_map>

#include "MW_API_Constants.hpp"
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
  MW_Master(int, int, const std::list<std::shared_ptr<Work>> &, double);
  MW_Master(int, int); // Restores from checkpoint
  MW_Master(int, int, double);
  ~MW_Master();
  static std::shared_ptr<MW_Master> restore(int, int);
  static std::shared_ptr<MW_Master> restore(int, int, double);
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

  double lastCheckpoint;
  double lastHeartbeat;
  double delayUntil;
  MW_Random random;

  void initializeWorkerMap();
  void initializeWorkFromCheckpoint();
  void initializeResultFromCheckpoint();
  void initializeWorkToDoFromCheckpoint();
  void keepInTouch();
  int nextWorker();
  void checkOnWorkers();
  void send_done();
  void send(int);
  void receive();
  void receiveHeartbeat();
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
