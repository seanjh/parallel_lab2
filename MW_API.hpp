#ifndef __MW__MW_API__
#define __MW__MW_API__

#include <list>
#include <memory>

#include "MW_API_Types.hpp"
#include "Work.hpp"
#include "Result.hpp"

using namespace std;

extern const int MASTER_PROCESS_ID;

class MW_API {
public:
    virtual list<shared_ptr<Work>> &work() = 0;
    virtual int results(shared_ptr<list<shared_ptr<Result>>>) = 0;
};

void MW_Run(int argc, char* argv[], shared_ptr<MW_API>);

#endif
