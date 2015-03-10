#ifndef __MW__MASTER__
#define __MW__MASTER__

#include "MW_API.hpp"
#include "MW_Process.hpp"
#include <list>
#include "Work.hpp"

class MW_Master : public MW_Process {
  int id;
  int world_size;
  // MW_API *app;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;
  std::list<int> *workers;

public:
  // MW_Master()
  MW_Master(const int myid, const int sz, std::list<Work *> *workToDo);
  void masterLoop();
  ~MW_Master();
private:
  // virtual void send_one(int worker_id);
  // virtual void send_work();
  // virtual void receive_result();
};

#endif /* defined(__MW__MASTER__) */
