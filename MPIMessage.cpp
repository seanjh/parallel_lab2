#include "MPIMessage.hpp"
#include "DivisorResult.hpp"
#include "DivisorWork.hpp"
#include <memory>

std::shared_ptr<Result> MPIMessage::deserializeResult()
{
  return DivisorResult::deserialize(message);
}

std::shared_ptr<Work> MPIMessage::deserializeWork()
{
  return DivisorWork::deserialize(message);
}
