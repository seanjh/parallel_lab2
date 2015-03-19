#ifndef __MW__MASTER__
#define __MW__MASTER__

#include <list>

#include "MW_Process.hpp"
#include "Work.hpp"
#include "Result.hpp"

class MW_Master : public MW_Process {
public:
  MW_Master(const int, const int, std::list<Work *> *);
  void master_loop();
  std::list<Result *> *getResults() { return results; };
  ~MW_Master();

private:
  int id;
  int world_size;
  std::list<Work *> *workToDo;
  std::list<Result *> *results;
  std::list<int> *workers;

  int nextWorker();
  void send_done();
  void send(int);
  enum MwTag receive();
  bool hasWorkersHasWork();
  bool hasWorkersNoWork();
  bool noWorkersHasWork();
  bool noWorkersNoWork();
  bool hasAllWorkers();
};

#endif /* defined(__MW__MASTER__) */
