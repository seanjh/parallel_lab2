#ifndef __MW__MW_API_TYPES__
#define __MW__MW_API_TYPES__

#define MAX_MESSAGE_SIZE 8092

typedef enum _MwTag
{
  WORK_TAG    = 1,
  DONE_TAG    = 2,
  HEARTBEAT   = 3,
  CHECKPOINT  = 4
} MWTag;

typedef enum MASC {
	Preempted = -1,
	Success,
	Failure
} MW_API_STATUS_CODE;

#endif
