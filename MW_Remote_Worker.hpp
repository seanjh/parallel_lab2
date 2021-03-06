#ifndef __MW__REMOTE_WORKER__
#define __MW__REMOTE_WORKER__

#include <unordered_map>

#include "MW_Monitor.hpp"
#include "Work.hpp"
#include "MW_API_Constants.hpp"

class MW_Remote_Worker {
public:
	MW_Remote_Worker(int);
	MW_Remote_Worker(const MW_Remote_Worker &);
	void markPending(MW_ID);
	void markCompleted(MW_ID);
	int workPendingCount();
	bool isAvailable();

	std::list<MW_ID> &getPendingWork();
	MW_Monitor heartbeatMonitor;
	const int id;

private:
	std::list<MW_ID> issuedWork;
};

#endif
