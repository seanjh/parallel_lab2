#ifndef __MW__MW_API_TYPES__
#define __MW__MW_API_TYPES__

#define MAX_MESSAGE_SIZE 8092

typedef enum _MwTag
{
  WORK_TAG,
  DONE_TAG,
  HEARTBEAT_TAG,
  CHECKPOINT_TAG,
  CHECKPOINT_DONE_TAG
} MWTag;

typedef enum MASC {
	Preempted = -1,
	Success,
	Failure
} MW_API_STATUS_CODE;

typedef unsigned long MW_ID;
const double HEARTBEAT_PERIOD = .5;

#endif
