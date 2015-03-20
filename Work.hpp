#ifndef __MW__WORK__
#define __MW__WORK__

#include <string>
#include "Result.hpp"
#include "MW_Semaphore.hpp"
#include "MW_API_Types.hpp"
#include <memory>


class Result;

class Work {
public:
    // Must be present, even if it does nothing.

    //This needs to destroy all of its children to prevent memory leaks
    virtual ~Work() {};
public:
    virtual MW_API_STATUS_CODE compute(const MW_Semaphore &) = 0;
    virtual std::shared_ptr<Result> result() = 0;
    virtual std::string *serialize() = 0;
    static Work *deserialize(const std::string &){return NULL;}
    // You could add more to the public interface, if you wanted to.
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
};


#endif /* defined(__MW__WORK__) */
