#include <iostream>
#include <mpi.h>

#include "MW_Monitor.hpp"
#include "MW_API_Types.hpp"

using namespace std;

MW_Monitor::MW_Monitor(int proc_ic, double time_period) : period(time_period), id(proc_ic)
{
  // heartbeats = new list<double>;
  dead = false;
  addHeartbeat();
}

// MW_Monitor &MW_Monitor::operator= ( const MW_Monitor & m):
// {
// 	return MW_Monitor(m);
// }

MW_Monitor::MW_Monitor(const MW_Monitor &m) : MW_Monitor(m.id, m.period)
{

}

bool MW_Monitor::isAlive()
{
  // if (!dead) {
  //   dead = (MPI::Wtime() - heartbeats.back()) > period;
  // }
  // return dead;
  return (MPI::Wtime() - heartbeats.back()) < period;
}

void MW_Monitor::addHeartbeat()
{
  double heartbeat = MPI::Wtime();
  heartbeats.push_back(heartbeat);
}

void MW_Monitor::sendHeartbeat(bool masterFlag)
{
  cout<<"Sending heartbeat to " << id <<endl;
  MPI::COMM_WORLD.Send(
    (void *) &masterFlag,
    1,
    MPI::BOOL,
    id,
    HEARTBEAT_TAG
  );
}
