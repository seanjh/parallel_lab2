#ifndef __MW__WORK__
#define __MW__WORK__

#include <string>
#include "Result.hpp"
#include "MW_Object.hpp"
#include "MW_API_Constants.hpp"
#include <memory>


class Result;

class Work {
public:
    // Must be present, even if it does nothing.

    //This needs to destroy all of its children to prevent memory leaks
    virtual ~Work() {};
public:
    virtual MW_API_STATUS_CODE compute(const MW_Object &) = 0;
    virtual std::shared_ptr<Result> result() = 0;
    virtual std::string *serialize() = 0;
    static std::shared_ptr<Work> deserialize(const std::string &){return nullptr;}
    // You could add more to the public interface, if you wanted to.
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
};


#endif /* defined(__MW__WORK__) */
