#include <assert.h>
#include <iostream>
#include <mpi.h>

#include "MW_API.hpp"
#include "MW_Process.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"

#define MASTER_PROCESS_ID 0

void MW_Run(int argc, char* argv[], MW_API *app)
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  double starttime, endtime;

  MPI::COMM_WORLD.Barrier();

  if (myid == MASTER_PROCESS_ID) {


    MW_Master *proc = new MW_Master(myid, sz, app->work());

    starttime = MPI::Wtime();
    proc->workLoop();
    endtime = MPI::Wtime();

    app->results(proc->getResults());

    delete proc;

  } else {
    MW_Worker *proc = new MW_Worker(myid, 0);

    proc->worker_loop();

    // Result *result;
    // enum MwTag message_tag;
    // while (1) {

    //   message_tag = proc->receive();

    //   if (message_tag == WORK_TAG) {

    //     proc->doWork();
    //     proc->send();

    //   } else if (message_tag == DONE_TAG) {
    //     // std::cout << "P:" << proc->id << " IS DONE\n";
    //     break;
    //   } else {
    //     std::cout << "WTF happened here\n";
    //     assert(0);
    //   }
    // }

    delete proc;
  }

  if (myid == MASTER_PROCESS_ID) {
    double elapsed = endtime - starttime;
    std::cout << "Completed in " << elapsed * 1000 << " ms\n";
  }

  MPI::Finalize();
}
