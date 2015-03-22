#include "MW_Remote_Worker.hpp"
#include <iostream>

MW_Remote_Worker::MW_Remote_Worker(int _id) : id(_id), heartbeatMonitor(MW_Monitor(_id, 5.0*HEARTBEAT_PERIOD))
{

}

MW_Remote_Worker::MW_Remote_Worker(const MW_Remote_Worker &rw): MW_Remote_Worker(rw.id)
{

}

// MW_Remote_Worker & MW_Remote_Worker::operator= ( const MW_Remote_Worker & rw)
// {
// 	return MW_Remote_Worker(rw);
// }

void MW_Remote_Worker::markPending(MW_ID work_id)
{
	// std::cout << "Marking " << work_id << " as pending" <<std::endl;
	issuedWork.push_back(work_id);
}

void MW_Remote_Worker::markCompleted(MW_ID work_id)
{
	// std::cout << "Marking " << work_id << " as completed" <<std::endl;
	issuedWork.remove(work_id);
}

int MW_Remote_Worker::workPendingCount()
{
	return issuedWork.size(); 
}

std::list<MW_ID> &MW_Remote_Worker::getPendingWork()
{
	return issuedWork;
}

bool MW_Remote_Worker::isAvailable()
{
	bool alive = heartbeatMonitor.isAlive();
	// if(!alive)
	// 	std::cout<< id << " is NOT alive" << std::endl;
	bool empty = issuedWork.empty();
	// if(empty)
	// 	std::cout<< id << " is empty" << std::endl;
	// else
	// 	std::cout<< id << " is NOT empty" << std::endl;
	return  alive && empty;
}

