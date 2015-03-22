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
// #include "boost/asio.hpp"

class MW_Worker {


public:
// <<<<<<< HEAD
  MW_Worker(const int, const int, const int);
// =======
//   MW_Worker(int, int, int);
// >>>>>>> sean-lab3
  //void send(int) { send(); };
  //void send();
  //enum MwTag receive();
  //void doWork();
  void worker_loop();
  ~MW_Worker();

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
};

#endif /* defined(__MW__WORKER__) */
