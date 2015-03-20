#ifndef __MW__MONITOR__
#define __MW__MONITOR__

#include <list>

class MW_Monitor {
public:
  MW_Monitor(int, double);
  bool isAlive();
  void addHearbeat();

private:
  int id;
  bool dead;
  double period;
  std::list<double> *heartbeats;
};

#endif //__MW__MONITOR__
