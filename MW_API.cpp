#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include "MW_Process.hpp"

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
  std::shared_ptr<MW_Process> p;

  if (myid == MASTER_PROCESS_ID) {
    // MW_Master *proc = new MW_Master(myid, sz, app->work());
    auto proc = std::make_shared<MW_Master>(myid, sz, app->work());
    // std::shared_ptr<MW_Master> proc = std::shared_ptr<MW_Master>(new MW_Master(myid, sz, app->work()));
    // auto proc = MW_Master::restore(myid, sz);

    starttime = MPI::Wtime();

    proc->master_loop();

    endtime = MPI::Wtime();


    app->results(proc->getResults());

    p = proc;

    // delete proc;

  } else {
    // MW_Worker *proc = new MW_Worker(myid, MASTER_PROCESS_ID);
    auto proc = std::make_shared<MW_Worker>(myid, MASTER_PROCESS_ID, sz);

    // std::shared_ptr<MW_Worker> proc = std::shared_ptr<MW_Worker>(new MW_Worker(myid, MASTER_PROCESS_ID, sz));
    bool transitionToMaster = proc->worker_loop();
    if(transitionToMaster)
    {
      auto proc = std::make_shared<MW_Master>(myid, sz, app->work());
      // std::shared_ptr<MW_Master> proc = MW_Master::restore(myid, sz);

      proc->master_loop();

      endtime = MPI::Wtime();
      app->results(proc->getResults());
    }
    p = proc;
    // delete proc;
  }

  assert(p != nullptr);
  assert(p->isMaster() || !p->isMaster());
  if (p->isMaster()) {
  // if (myid == MASTER_PROCESS_ID) {
    std::cout << "My Process ID is " << myid << std::endl;
    double elapsed = endtime - starttime;
    std::cout << "Completed in " << elapsed * 1000 << " ms\n";
  }

  MPI::Finalize();
}
