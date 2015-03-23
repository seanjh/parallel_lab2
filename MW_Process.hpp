#ifndef __MW__PROCESS__
#define __MW__PROCESS__

class MW_Process {
public:
	virtual bool isMaster() = 0;
	virtual std::shared_ptr<std::list<std::shared_ptr<Result>>> getResults() = 0;
};

#endif
