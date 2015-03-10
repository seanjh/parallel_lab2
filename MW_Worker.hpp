#ifndef __MW__WORKER__
#define __MW__WORKER__

#include "MW_Process.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include "MW_API.hpp"
#include <list>

class MW_Worker : public MW_Process {


public:
  int id;
  int master_id;
  MW_API *app;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;

  // MW_Worker(const int id, const int master);
  MW_Worker(const int, const int, MW_API *);
  virtual int receiveWork();
  virtual void sendResults();
};

#endif /* defined(__MW__WORKER__) */
