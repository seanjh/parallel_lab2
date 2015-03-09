#ifndef __MW__WORKER__
#define __MW__WORKER__

#include "MW_Process.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <list>

class MW_Worker : public MW_Process {
  int id;
  int master_id;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;

public:
  MW_Worker(const int id, const int master);
  virtual void receiveWork();
  virtual void sendResults();
};

#endif /* defined(__MW__WORKER__) */
