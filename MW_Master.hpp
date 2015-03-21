#ifndef __MW__MASTER__
#define __MW__MASTER__

#include <list>

// #include "MW_Process.hpp"
#include "MW_API_Types.hpp"
#include "MW_Monitor.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <unordered_map>
#include <memory>
#include "MW_Remote_Worker.hpp"

class MW_Master {
public:
  MW_Master(int, int, const std::list<std::shared_ptr<Work>> &);

  //TODO: Add new constructor for worker->master transition, add work to do and results parameters
  void master_loop();
  std::shared_ptr<std::list<std::shared_ptr<Result>>> getResults();
  ~MW_Master();

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
};

#endif /* defined(__MW__MASTER__) */
