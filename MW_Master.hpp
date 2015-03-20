#ifndef __MW__MASTER__
#define __MW__MASTER__

#include <list>

// #include "MW_Process.hpp"
#include "MW_API_Types.hpp"
#include "MW_Monitor.hpp"
#include "Work.hpp"
#include "Result.hpp"

class MW_Master {
public:
  MW_Master(const int, const int, const std::list<std::shared_ptr<Work>> &);
  void master_loop();
  std::list<Result *> *getResults() { return results; };
  ~MW_Master();

private:
  int id;
  int world_size;
  const std::list<std::shared_ptr<Work>> &work;
  std::list<std::shared_ptr<Work>> *workToDo;
  std::list<Result *> *results;
  // std::list<MW_Monitor *> *workers;
  std::list<int> *workers;

  int nextWorker();
  void checkOnWorkers();
  void send_done();
  void send(int);
  void receive();
  void process_result(int, int, char *);
  void process_heartbeat(int);
  bool hasWorkersHasWork();
  bool hasWorkersNoWork();
  bool noWorkersHasWork();
  bool noWorkersNoWork();
  bool hasAllWorkers();
};

#endif /* defined(__MW__MASTER__) */
