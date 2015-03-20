#ifndef __MPI__MMESSAGE__
#define __MPI__MMESSAGE__

#include <iostream>
#include <string>
#include "Result.hpp"
#include "Work.hpp"

class MPIMessage
{
  std::string message;

public:
  MPIMessage() : message("") {}
  MPIMessage(const std::string & str) : message(str) {}
  MPIMessage(const char * str) : message(str) {}
  MPIMessage(const MPIMessage & m) : message(m.message) {}

  ~MPIMessage() {}

  std::string & to_string()
  {
    return message;
  }

  // std::ostream &operator<<(std::ostream &stream, const MPIMessage & m)
  // {
  //   stream << m.message;
  //   return stream;
  // }

  virtual std::shared_ptr<Result> deserializeResult();
  virtual std::shared_ptr<Work> deserializeWork();
};

#endif
