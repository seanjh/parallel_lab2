#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <list>

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MW_Timer.hpp"
#include <unordered_map>
#include <memory>
#include "MW_Monitor.hpp"
#include "MW_Random.hpp"
#include "MW_Process.hpp"
// #include "boost/asio.hpp"

class MW_Worker : public MW_Process {


public:
  MW_Worker(const int, const int, const int);
  bool worker_loop();
  ~MW_Worker();
  virtual bool isMaster() {return false;};

private:
  int id;
  int master_id;
  int world_size;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> workToDo;
  std::unordered_map<MW_ID, std::shared_ptr<Result>> results;
  std::shared_ptr<MW_Monitor> masterMonitor;
  std::unordered_map<int, std::shared_ptr<MW_Monitor>> otherWorkersMonitorMap;

  MW_Timer preemptionTimer;
  MW_Random random;
  double nextMasterCheckTime;
  bool heardFromMaster;

  MWTag receive();
  // void doWork();
  // void send();
  // void send(int) { send(); };
  bool hasWork() {return !workToDo.empty();};
  void send(MW_ID result_id, std::shared_ptr<Result> result);
  bool shouldSendHeartbeat();
  void sendHeartbeat();
  void broadcastHeartbeat();
  double lastHeartbeat;
  // bool hasMaster();
  bool shouldCheckOnMaster();
  bool checkOnMaster();
};

#endif /* defined(__MW__WORKER__) */
