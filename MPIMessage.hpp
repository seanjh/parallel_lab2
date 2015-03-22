#ifndef __MPI__MMESSAGE__
#define __MPI__MMESSAGE__

#include <iostream>
#include <string>

#include "Result.hpp"
#include "Work.hpp"

using namespace std;

class MPIMessage
{
  string message;

public:
  MPIMessage() : message("") {}
  MPIMessage(const string & str) : message(str) {}
  MPIMessage(const char * str) : message(str) {}
  MPIMessage(const MPIMessage & m) : message(m.message) {}

  ~MPIMessage() {}

  string & to_string()
  {
    return message;
  }

  // ostream &operator<<(ostream &stream, const MPIMessage & m)
  // {
  //   stream << m.message;
  //   return stream;
  // }

  virtual shared_ptr<Result> deserializeResult();
  virtual shared_ptr<Work> deserializeWork();
};

#endif
