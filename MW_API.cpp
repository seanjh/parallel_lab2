#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include <mpi.h>

void MW_Run(int argc, char* argv[], MW_API *app)
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  // master creates work universe
  if (myid == 0) {
    MW_Master *proc = new MW_Master(myid, sz, app);
    proc->send_one(1);
    proc->receive_result();
  } else {
    MW_Worker *proc = new MW_Worker(myid, 0);
    proc->receiveWork();
    proc->sendResults();
  }

  // master sends work
  // workers receive work
  // workers sends results
  // master receives results

  MPI::Finalize ();
}
