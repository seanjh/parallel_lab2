#include "MPIMessage.hpp"
#include "DivisorResult.hpp"
#include "DivisorWork.hpp"

Result * MPIMessage::deserializeResult()
{
  return DivisorResult::deserialize(message);
}

Work * MPIMessage::deserializeWork()
{
  return DivisorWork::deserialize(message);
}
