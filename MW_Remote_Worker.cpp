#include "MW_Remote_Worker.hpp"

MW_Remote_Worker::MW_Remote_Worker(int _id) : id(_id), heartbeatMonitor(MW_Monitor(_id, HEARTBEAT_PERIOD))
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
	issuedWork.push_back(work_id);
}

void MW_Remote_Worker::markCompleted(MW_ID work_id)
{
	issuedWork.remove(work_id);
}

int MW_Remote_Worker::workPendingCount()
{
	return issuedWork.size(); 
}

const std::list<MW_ID> &MW_Remote_Worker::getPendingWork()
{
	return issuedWork;
}

bool MW_Remote_Worker::isAvailable()
{
	return heartbeatMonitor.isAlive() && issuedWork.empty();
}

