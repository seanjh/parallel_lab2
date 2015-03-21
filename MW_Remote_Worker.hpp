#ifndef __MW__REMOTE_WORKER__
#define __MW__REMOTE_WORKER__

#include "MW_Monitor.hpp"
#include <unordered_map>
#include "Work.hpp"
#include "MW_API_Types.hpp"

class MW_Remote_Worker {
public:
	MW_Remote_Worker(int);
	MW_Remote_Worker(const MW_Remote_Worker &);
	// MW_Remote_Worker &operator= ( const MW_Remote_Worker & );
	void markPending(MW_ID);
	void markCompleted(MW_ID);
	int workPendingCount();
	bool isAvailable();
	const std::list<MW_ID> &getPendingWork();

	MW_Monitor heartbeatMonitor;

private:
	const int id;
	std::list<MW_ID> issuedWork;
	
};

#endif