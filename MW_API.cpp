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

  // MPI_Comm_size (MPI_COMM_WORLD, &sz);
  // MPI_Comm_rank (MPI_COMM_WORLD, &myid);

  // set up masters, workers
  if (myid == 0) {
    MW_Master *proc = new MW_Master(myid, sz, app);
  } else {
    MW_Worker *proc = new MW_Worker(myid, 0);
  }


  // master creates work universe
  // master sends work
  // workers receive work
  // workers sends results
  // master receives results

  MPI::Finalize ();
}
