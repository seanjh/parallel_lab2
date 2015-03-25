#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include "MW_Process.hpp"

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

  starttime = MPI::Wtime();

  
  // int master_id = MASTER_PROCESS_ID;

  if (myid == MASTER_PROCESS_ID) {
    std::shared_ptr<MW_Master> proc = std::shared_ptr<MW_Master>(new MW_Master(myid, sz, app->work()));
   
    p = proc;

    proc->master_loop();

    // bool done;
    // while (!done) {
    //   done = 
    // }
    // auto proc = MW_Master::restore(myid, sz);

    endtime = MPI::Wtime();

    // app->results(proc->getResults());

    // p = proc;

  } else {
    auto proc = std::make_shared<MW_Worker>(myid, MASTER_PROCESS_ID, sz);

    // std::shared_ptr<MW_Worker> proc = std::shared_ptr<MW_Worker>(new MW_Worker(myid, MASTER_PROCESS_ID, sz));
    // bool transitionToMaster = proc->worker_loop();

    bool done = false;
    while (!done) {
      done = proc->worker_loop();
      p = proc;
      bool shouldTransitionToMaster = proc->transitionMaster();

      if (!done && shouldTransitionToMaster) {
        // The Master died :(
        std::cout << "P" << myid << ": Make me master!\n";
        std::shared_ptr<MW_Master> proc = MW_Master::restore(myid, sz);
        done = proc->master_loop();

        std::cout << "P" << myid << ": total results: " << proc->getResults()->size() << "\n";
        p = proc;

      } else if (!done) {
        std::cout << "P" << myid << ": Master failed. Not done yet!\n";
      }

      std::cout << "P" << myid << ": finishing\n";

      // p = proc;
      endtime = MPI::Wtime();
    }
  }

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
