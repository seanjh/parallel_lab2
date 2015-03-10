#ifndef __MW__WORK__
#define __MW__WORK__

#include <string>
#include "Result.hpp"


class Result;
    
class Work {
public:
    // Must be present, even if it does nothing.
    virtual ~Work();
public:
    virtual Result *compute() = 0;
    virtual std::string *serialize() = 0;
    static Work *deserialize(const std::string &){return NULL;}
    // You could add more to the public interface, if you wanted to.
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
};


#endif /* defined(__MW__WORK__) */