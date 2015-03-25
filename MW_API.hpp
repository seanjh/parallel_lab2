#ifndef __MW__MW_API__
#define __MW__MW_API__

#include <list>
#include <memory>

#include "MW_API_Constants.hpp"
#include "Work.hpp"
#include "Result.hpp"

extern const int MASTER_PROCESS_ID;

class MW_API {
public:
    virtual std::list<std::shared_ptr<Work>> &work() = 0;
    virtual int results(std::shared_ptr<std::list<std::shared_ptr<Result>>>) = 0;
};

void MW_Run(int argc, char* argv[], MW_API *app);

#endif
