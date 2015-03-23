#include <mpi.h>

#include "MW_Monitor.hpp"
#include "MW_API_Types.hpp"
#include <iostream>

MW_Monitor::MW_Monitor(int proc_ic, double time_period) : period(time_period), id(proc_ic)
{
  // heartbeats = new list<double>;
  dead = false;
  // addHeartbeat();
}
//
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

void MW_Monitor::dump()
{
  for(auto it=heartbeats.begin(); it!=heartbeats.end(); it++)
    std::cout<<*it<<std::endl;
}

void MW_Monitor::sendHeartbeat(bool masterFlag)
{
  // std::cout<<"Sending heartbeat to " << id <<std::endl;
  char m = masterFlag ? 1 : 0;
  MPI::COMM_WORLD.Send(
    (void *) &m,
    1,
    MPI::CHAR,
    id,
    HEARTBEAT_TAG
  );
}
