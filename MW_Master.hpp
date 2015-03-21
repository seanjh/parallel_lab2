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

extern const double       CHECKPOINT_PERIOD;
extern const std::string  WORK_CHECKPOINT_FILENAME;
extern const std::string  RESULTS_CHECKPOINT_FILENAME;

class MW_Master {
public:
  MW_Master(int, int, const std::list<std::shared_ptr<Work>> &);
  //TODO: Add new constructor for worker->master transition, add work to do and results parameters
  MW_Master(int, int); // Restores from checkpoint
  void master_loop();
  std::shared_ptr<std::list<std::shared_ptr<Result>>> getResults();
  ~MW_Master();
  static std::shared_ptr<MW_Master> restore(int, int);

private:
  int id;
  int world_size;
  //const std::list<std::shared_ptr<Work>> &work;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> work;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> workToDo;
  std::unordered_map<MW_ID, std::shared_ptr<Result>> results;
  // std::list<MW_Monitor *> *workers;
  // std::list<int> *workers;
  std::unordered_map<int, std::shared_ptr<MW_Remote_Worker>> workerMap;
  // std::list<int> freeWorkers;

  double lastCheckpoint;
  double lastHeartbeat;

  void initializeWorkerMap();
  int nextWorker();
  void checkOnWorkers();
  void send_done();
  void send(int);
  void receive();
  void process_result(int, int, char *);
  void process_heartbeat(int);
  void process_checkpoint_done(int);
  bool hasWorkersHasWork();
  bool hasWorkersNoWork();
  bool noWorkersHasWork();
  bool noWorkersNoWork();
  bool hasAllWorkers();
  void performCheckpoint();
  bool shouldCheckpoint();
  bool shouldSendHeartbeat();
  void sendHeartbeat();

};

#endif /* defined(__MW__MASTER__) */
