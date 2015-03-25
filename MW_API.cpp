#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include "MW_Process.hpp"

const int INITIAL_MASTER_PROCESS_ID = 0;

void MW_Run(int argc, char* argv[], MW_API *app)
{
  int sz, myid;
  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  double starttime, endtime;
  bool done;
  std::shared_ptr<MW_Process> p;

  MPI::COMM_WORLD.Barrier();
  starttime = MPI::Wtime();


  double prob = SEND_FAILURE_PROBABILITY;
  if(argc > 3)
    prob = std::stod(std::string(argv[3]));

  if (myid == INITIAL_MASTER_PROCESS_ID) {
    // MASTER
    auto proc = std::make_shared<MW_Master>(myid, sz, app->work(), prob);
    p = proc;

    while (!done) {
      done = proc->master_loop();
    }

    endtime = MPI::Wtime();

  } else {
    // WORKERS
    auto proc = std::make_shared<MW_Worker>(myid, INITIAL_MASTER_PROCESS_ID, sz, prob);
    p = proc;

    while (!done) {
      // The Master died :(
      done = proc->worker_loop();
      bool shouldTransitionToMaster = proc->transitionMaster(done);

      if (!done && shouldTransitionToMaster) {
        // The next Master gets in here.
        std::shared_ptr<MW_Master> proc = MW_Master::restore(myid, sz, prob);
        done = proc->master_loop();
        p = proc;
      } else if (!done) {
        // Remaining Workers just re-enter worker_loop. Nothing to do.
      }

      endtime = MPI::Wtime();
    }
  }

  std::cout << "P" << myid << ": finished\n";
  assert(p != nullptr);
  assert(p->isMaster() || !p->isMaster());
  if (p->isMaster()) {
    app->results(p->getResults());
    std::cout << "I'm the MASTER and my Process ID is " << myid << std::endl;
    double elapsed = endtime - starttime;
    std::cout << "Completed in " << elapsed * 1000 << " ms\n";
  }

  MPI::Finalize();
}
