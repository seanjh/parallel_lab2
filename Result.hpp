#ifndef __MW__RESULT__
#define __MW__RESULT__

#include <string>
#include <memory>

#include "Work.hpp"

using namespace std;

class Work;

class Result {
public:
    // Must be present, even if it does nothing.
	//copy constructor

    //This needs to destroy all of its children to prevent memory leaks
    virtual ~Result() {};
    virtual Result* clone() const = 0;
public:

  virtual shared_ptr<string> serialize() = 0;
  static shared_ptr<Result> deserialize(const string &)
  {
    shared_ptr<Result> ptr (nullptr);
    return ptr;
  }
    // You could add more to the public interface, if you wanted to.
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
};


#endif /* defined(__MW__RESULT__) */
