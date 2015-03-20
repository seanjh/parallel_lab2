#include <mpi.h>

#include "MW_Monitor.hpp"

MW_Monitor::MW_Monitor(int proc_ic, double time_period) : period(time_period), id(proc_ic)
{
  heartbeats = new list<double>;
  dead = false;
  addHeartbeat();
}

bool MW_Monitor::isAlive()
{
  if (!dead) {
    dead = (MPI::Wtime() - heartbeats->back()) > period;
  }
  return dead;
}

void MW_Monitor::addHearbeat()
{
  double heartbeat = MPI::Wtime();
  heartbeats->push_back(heartbeat);
}
