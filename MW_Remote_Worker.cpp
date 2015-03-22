#include <iostream>

#include "MW_Remote_Worker.hpp"

using namespace std;

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
	// cout << "Marking " << work_id << " as pending" <<endl;
	issuedWork.push_back(work_id);
}

void MW_Remote_Worker::markCompleted(MW_ID work_id)
{
	// cout << "Marking " << work_id << " as completed" <<endl;
	issuedWork.remove(work_id);
}

int MW_Remote_Worker::workPendingCount()
{
	return issuedWork.size();
}

const list<MW_ID> &MW_Remote_Worker::getPendingWork()
{
	return issuedWork;
}

bool MW_Remote_Worker::isAvailable()
{
	bool alive = heartbeatMonitor.isAlive();
	if(!alive)
		cout<< id << " is NOT alive" << endl;
	bool empty = issuedWork.empty();
	// if(empty)
	// 	cout<< id << " is empty" << endl;
	// else
	// 	cout<< id << " is NOT empty" << endl;
	return  alive && empty;
}
