#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <unordered_map>
#include <memory>
#include <list>

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MW_Timer.hpp"
// #include "boost/asio.hpp"

using namespace std;

class MW_Worker {


public:
  MW_Worker(int, int, int);
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
  unordered_map<MW_ID, shared_ptr<Work>> workToDo;
  unordered_map<MW_ID, shared_ptr<Result>> results;

  MW_Timer preemptionTimer;

  MWTag receive();
  // void doWork();
  // void send();
  // void send(int) { send(); };
  bool hasWork() {return !workToDo.empty();};
  void send(MW_ID result_id, shared_ptr<Result> result);
};

#endif /* defined(__MW__WORKER__) */
