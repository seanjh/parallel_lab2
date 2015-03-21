#ifndef __MW__MONITOR__
#define __MW__MONITOR__

#include <list>

class MW_Monitor {
public:
  MW_Monitor(int, double);
  MW_Monitor(const MW_Monitor &);
  // MW_Monitor &operator= ( const MW_Monitor & );
  bool isAlive();
  void addHeartbeat();

private:
  const int id;
  bool dead;
  const double period;
  std::list<const double> heartbeats;
};

#endif //__MW__MONITOR__
