#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"

// #define MASTER_PROCESS_ID 0
const int MASTER_PROCESS_ID = 0;

void MW_Run(int argc, char* argv[], MW_API *app)
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  double starttime, endtime;

  MPI::COMM_WORLD.Barrier();

  if (myid == MASTER_PROCESS_ID) {
    // MW_Master *proc = new MW_Master(myid, sz, app->work());
    auto proc = std::make_shared<MW_Master>(myid, sz, app->work());
    // auto proc = MW_Master::restore(myid, sz);

    starttime = MPI::Wtime();

    proc->master_loop();

    endtime = MPI::Wtime();


    app->results(proc->getResults());

    // delete proc;

  } else {
    // MW_Worker *proc = new MW_Worker(myid, MASTER_PROCESS_ID);
    auto proc = std::make_shared<MW_Worker>(myid, MASTER_PROCESS_ID, sz);

    proc->worker_loop();

    // delete proc;
  }

  if (myid == MASTER_PROCESS_ID) {
    double elapsed = endtime - starttime;
    std::cout << "Completed in " << elapsed * 1000 << " ms\n";
  }

  MPI::Finalize();
}
