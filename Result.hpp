#ifndef __MW__RESULT__
#define __MW__RESULT__

#include <string>
#include "Work.hpp"


class Work;
    
class Result {
public:

    virtual ~Result();
public:
    virtual std::string *serialize() = 0;
    static Result *deserialize(const std::string &){return NULL;}
    // You could add more to the public interface, if you wanted to.
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
};


#endif /* defined(__MW__RESULT__) */