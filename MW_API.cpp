#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include <mpi.h>

void MW_API::MW_Run(int argc, char* argv[])
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  // master creates work universe
  if (myid == 0) {
    MW_Master *master = new MW_Master(myid, sz, work());
    master->masterLoop();

    //all work is completed at this point






    // proc->send_one(1);
    // proc->receive_result();
  } else {
    MW_Worker *proc = new MW_Worker(myid, 0);
    proc->workerLoop();
    // proc->receiveWork();
    // proc->sendResults();
  }

  // master sends work
  // workers receive work
  // workers sends results
  // master receives results

  MPI::Finalize ();
}
