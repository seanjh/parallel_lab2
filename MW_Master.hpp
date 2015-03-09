#ifndef __MW__MASTER__
#define __MW__MASTER__

#include "MW_API.hpp"
#include "MW_Process.hpp"
#include <list>

class MW_Master : public MW_Process {
  int id;
  int world_size;
  MW_API *app;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;
  std::list<int> *workers;

public:
  // MW_Master()
  MW_Master(const int myid, const int sz, MW_API *app);
  virtual void send_work(const int worker_id);
  virtual void receive_result();
};

#endif /* defined(__MW__MASTER__) */
