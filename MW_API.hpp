#ifndef __MW__MW_API__
#define __MW__MW_API__

#include "Work.hpp"
#include "Result.hpp"
#include <list>

class MW_API {
public:
    virtual std::list<Work *> *work() = 0;
    virtual int results(std::list<Result *> *) = 0;
    void MW_Run(int argc, char* argv[]);

private:
	// std::list<Work *> *workToDo;
	// std::list<Result *> *results;

};

#endif
