#include <memory>

#include "MPIMessage.hpp"
#include "DivisorResult.hpp"
#include "DivisorWork.hpp"

using namespace std;

shared_ptr<Result> MPIMessage::deserializeResult()
{
  return DivisorResult::deserialize(message);
}

shared_ptr<Work> MPIMessage::deserializeWork()
{
  return DivisorWork::deserialize(message);
}
