#ifndef __MW__PROCESS__
#define __MW__PROCESS__

#include "mpi.h"

// typedef enum WorkTag
// {
//   WORK_TAG  = 1;
//   DONE_TAG  = 2;
// } WorkTag;

#define MAX_MESSAGE_SIZE 8092


class MW_Process {
public:
  static const int WORK_TAG = 1;
  static const int DONE_TAG = 2;
  // static const int BUFF_SIZE_BYTES = 1000;
  // virtual WorkTag receive() = 0;
  // virtual void send(int) = 0;
protected:
  MW_Process() {}
};

#endif /* defined(__MW__PROCESS__) */
