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

using namespace std;

extern const double       CHECKPOINT_PERIOD;
extern const string  WORK_CHECKPOINT_FILENAME;
extern const string  RESULTS_CHECKPOINT_FILENAME;

class MW_Master {
public:
  MW_Master(int, int, const list<shared_ptr<Work>> &);
  MW_Master(int, int); // Restores from checkpoint
  void master_loop();
  shared_ptr<list<shared_ptr<Result>>> getResults();
  ~MW_Master();
  static shared_ptr<MW_Master> restore(int, int);

private:
  int id;
  int world_size;
  //const list<shared_ptr<Work>> &work;
  unordered_map<MW_ID, shared_ptr<Work>> work;
  unordered_map<MW_ID, shared_ptr<Work>> workToDo;
  unordered_map<MW_ID, shared_ptr<Result>> results;
  // list<MW_Monitor *> *workers;
  // list<int> *workers;
  unordered_map<int, shared_ptr<MW_Remote_Worker>> workerMap;
  // list<int> freeWorkers;

  double lastCheckpoint;
  double lastHeartbeat;

  void initializeWorkerMap();
  void initializeWorkFromCheckpoint();
  void initializeResultFromCheckpoint();
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
