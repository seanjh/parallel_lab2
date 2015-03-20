#ifndef __MW__MW_API__
#define __MW__MW_API__

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"
#include <list>
#include <memory>

class MW_API {
public:
    virtual std::list<std::shared_ptr<Work>> &work() = 0;
    virtual int results(std::shared_ptr<std::list<std::shared_ptr<Result>>>) = 0;
};

void MW_Run(int argc, char* argv[], MW_API *app);

#endif
