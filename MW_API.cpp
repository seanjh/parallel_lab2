#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include <mpi.h>
#include <iostream>

void MW_API::MW_Run(int argc, char* argv[])
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  // master creates work universe
  if (myid == 0) {

    // std::cout<<"top of list" << std::endl;
    const std::list<Work *> &work = this->work();
    // Work *pieceOfWork = work.front();
    // std::cout<<pieceOfWork <<std::endl;
    // std::cout << this->workSerializer(*pieceOfWork) <<std::endl;

    // std::cout<<"creating master" << std::endl;
    MW_Master *master = new MW_Master(myid, sz, work, this);
    master->masterLoop();

    //all work is completed at this point






    // proc->send_one(1);
    // proc->receive_result();
  } else {
    // MW_Worker *proc = new MW_Worker(myid, 0, this);
    // proc->workerLoop();
    // proc->receiveWork();
    // proc->sendResults();
  }

  // master sends work
  // workers receive work
  // workers sends results
  // master receives results

  MPI::Finalize ();
}
