#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <list>

#include "MW_API_Constants.hpp"
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
  bool transitionMaster(bool);
  std::shared_ptr<std::list<std::shared_ptr<Result>>> getResults()
  {
    auto ptr (nullptr);
    return ptr;
  }

private:
  int id;
  int master_id;
  int world_size;

  std::unordered_map<MW_ID, std::shared_ptr<Work>> workToDo;
  std::unordered_map<MW_ID, std::shared_ptr<Result>> results;
  std::shared_ptr<MW_Monitor> masterMonitor;
  std::unordered_map<int, std::shared_ptr<MW_Monitor>> otherWorkersMonitorMap;

  MW_Timer preemptionTimer;
  double nextMasterCheckTime;
  double lastHeartbeat;
  MW_Random random;

  MWTag receive( int, int, void *, int, int &, int &);
  MWTag receiveWork();
  MWTag receiveNewMaster(int &);
  MWTag receiveHeartbeat();
  MWTag receiveDone();

  void send(MW_ID result_id, std::shared_ptr<Result> result);
  void process_work(char*, int, int);
  void process_heartbeat(int);
  void process_new_master(int);
  bool hasWork() {return !workToDo.empty();};
  bool shouldSendHeartbeat();
  void sendHeartbeat();
  void broadcastHeartbeat();
  bool shouldCheckOnMaster();
  int findNextMasterId();
  bool isMasterAlive();
  void updateMasterCheckTime();
  void waitForNewMaster();
  bool checkOnMaster();
  std::shared_ptr<MW_Monitor> getMasterMonitor();
};

#endif /* defined(__MW__WORKER__) */
