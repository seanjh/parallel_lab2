#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <list>

#include "MW_Process.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MW_Semaphore.hpp"

class MW_Worker : public MW_Process {


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
  std::list<Work *> *workToDo;
  std::list<std::shared_ptr<Result>> *results;
  MW_Semaphore preemptionSemaphore;


  enum MwTag receive();
  void doWork();
  void send();
  void send(int) { send(); };
};

#endif /* defined(__MW__WORKER__) */
