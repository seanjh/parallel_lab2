#ifndef __MW__WORKER__
#define __MW__WORKER__

#include <list>

#include "MW_Process.hpp"
#include "Work.hpp"
#include "Result.hpp"

class MW_Worker : public MW_Process {


public:
  MW_Worker(const int, const int);
  void send(int) { send(); };
  void send();
  enum MwTag receive();
  void doWork();
  ~MW_Worker();

private:
  int id;
  int master_id;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;
};

#endif /* defined(__MW__WORKER__) */
