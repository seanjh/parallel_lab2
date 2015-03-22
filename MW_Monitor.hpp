#ifndef __MW__MONITOR__
#define __MW__MONITOR__

#include <list>

using namespace std;

class MW_Monitor {
public:
  MW_Monitor(int, double);
  MW_Monitor(const MW_Monitor &);
  // MW_Monitor &operator= ( const MW_Monitor & );
  bool isAlive();
  void addHeartbeat();
  void sendHeartbeat(bool masterFlag);

private:
  const int id;
  bool dead;
  const double period;
  list<double> heartbeats;
};

#endif //__MW__MONITOR__
