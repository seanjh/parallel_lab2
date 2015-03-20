#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <list>

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MW_Semaphore.hpp"
#include <unordered_map>

class MW_Worker {


public:
  MW_Worker(const int, const int);
  //void send(int) { send(); };
  //void send();
  //enum MwTag receive();
  //void doWork();
  void worker_loop();
  ~MW_Worker();

private:
  int id;
  int master_id;
  std::unordered_map<MW_ID, std::shared_ptr<Work>> workToDo;
  std::unordered_map<MW_ID, std::shared_ptr<Result>> results;

  MW_Semaphore preemptionSemaphore;


  MWTag receive();
  void doWork();
  void send();
  void send(int) { send(); };
};

#endif /* defined(__MW__WORKER__) */
