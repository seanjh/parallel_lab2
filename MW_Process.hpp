#ifndef __MW__PROCESS__
#define __MW__PROCESS__

#define MAX_MESSAGE_SIZE 8092

enum MwTag
{
  WORK_TAG  = 1,
  DONE_TAG  = 2
};

class MW_Process {
public:
  virtual void send(int) = 0;
  virtual enum MwTag receive() = 0;
// protected:
  // MW_Process() = 0;
};

#endif /* defined(__MW__PROCESS__) */
