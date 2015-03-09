#ifndef __MW__PROCESS__
#define __MW__PROCESS__

#include "mpi.h"

class MW_Process {
public:
  static const int WORK_TAG = 1;
  static const int DONE_TAG = 2;
  static const int BUFF_SIZE_BYTES = 1000;
protected:
  MW_Process() {}
};

#endif /* defined(__MW__PROCESS__) */
