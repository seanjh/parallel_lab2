#include "MW_API.hpp"
#include "MW_Master.hpp"
#include "MW_Worker.hpp"
#include <mpi.h>
#include <assert.h>
#include <iostream>

#define MASTER_PROCESS_ID 0

bool hasWorkersHasWork(MW_Master *master)
{
  return !master->workers->empty() && !master->workToDo->empty();
}

bool hasWorkersNoWork(MW_Master *master)
{
  return !master->workers->empty() && master->workToDo->empty();
}

bool noWorkersHasWork(MW_Master *master)
{
  return master->workers->empty() && !master->workToDo->empty();
}

bool noWorkersNoWork(MW_Master *master)
{
  return master->workers->empty() && master->workToDo->empty();
}

bool hasAllWorkers(MW_Master *master, int world_size)
{
  return master->workers->size() == world_size - 1;  // exclude self (master)
}

void MW_Run(int argc, char* argv[], MW_API *app)
{
  int sz, myid;

  MPI::Init(argc, argv);
  sz = MPI::COMM_WORLD.Get_size();
  myid = MPI::COMM_WORLD.Get_rank();

  MPI::COMM_WORLD.Barrier();
  if (myid == MASTER_PROCESS_ID) {

    int worker_id;
    MW_Master *proc = new MW_Master(myid, sz, app);
    while (1) {
      if (hasWorkersHasWork(proc)) {
        std::cout << "MASTER IS SENDING\n";
        worker_id = proc->workers->front();
        assert(worker_id != 0);
        proc->workers->pop_front();

        proc->send_one(worker_id);

      } else if (noWorkersHasWork(proc) || noWorkersNoWork(proc)) {
        std::cout << "MASTER IS WAITING FOR A RESULT\n";
        proc->receive_result();

      } else if (hasWorkersNoWork(proc)) {
        if (hasAllWorkers(proc, sz)) {
          std::cout << "MASTER IS DONE\n";
          proc->send_done();
          break;
        } else {
          proc->receive_result();
        }
      } else {
        std::cout << "WTF happened here\n";
        assert(0);
      }

    }
  } else {
    MW_Worker *proc = new MW_Worker(myid, 0, app);
    int message_tag;
    while (1) {
      message_tag = proc->receiveWork();

      if (message_tag == MW_Process::WORK_TAG) {
        // do stuff with the work
        std::cout << "P:" << proc->id << " Grabbing some workToDo\n";
        Work *work = proc->workToDo->front();
        assert(work != NULL);
        std::cout << "P:" << proc->id << " Work object is (" << work << ") \"" << work->serialize() << "\"\n";
        proc->workToDo->pop_front();

        std::cout << "P:" << proc->id << " Computing results.\n";
        Result *one_result = work->compute();
        assert(one_result != NULL);
        std::cout << "P:" << proc->id << " Result object is (" << one_result << ") \"" << one_result->serialize() << "\"\n";

        proc->results->push_back(one_result);

        proc->sendResults();

      } else {
        std::cout << "P:" << proc->id << " IS DONE\n";
        break;
      }
    }
  }


  // if (myid == 0) {
  //   MW_Master *proc = new MW_Master(myid, sz, app);
  //   proc->send_one(1);
  //   proc->receive_result();
  // } else {
  //   MW_Worker *proc = new MW_Worker(myid, 0, app);
  //   proc->receiveWork();
  //
  //   // do stuff with the work
  //   std::cout << "P:" << proc->id << " Grabbing some workToDo\n";
  //   Work *work = proc->workToDo->front();
  //   assert(work != NULL);
  //   std::cout << "P:" << proc->id << " Work object is (" << work << ") \"" << work->serialize() << "\"\n";
  //   proc->workToDo->pop_front();
  //
  //   std::cout << "P:" << proc->id << " Computing results.\n";
  //   Result *one_result = work->compute();
  //   assert(one_result != NULL);
  //   std::cout << "P:" << proc->id << " Result object is (" << one_result << ") \"" << one_result->serialize() << "\"\n";
  //
  //   proc->results->push_back(one_result);
  //   proc->sendResults();
  //
  // }

  MPI::Finalize ();
}
