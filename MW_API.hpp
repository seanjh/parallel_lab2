#ifndef __MW__MW_API__
#define __MW__MW_API__

#include "Work.hpp"
#include "Result.hpp"
#include <list>

class MW_API {
public:
    virtual std::list<Work *> *work() = 0;
    virtual int results(std::list<Result *> *) = 0;
    virtual std::string *workSerializer(Work &) = 0;
  	virtual Work *workDeserializer(const std::string &) = 0;
  	virtual std::string *resultSerializer(Result &) = 0;
  	virtual Result *resultDeserializer(const std::string &) = 0;
};

void MW_Run(int argc, char* argv[], MW_API *app);

#endif