#ifndef __MW__MW_API_TYPES__
#define __MW__MW_API_TYPES__

#define MAX_MESSAGE_SIZE 8092

typedef enum _MwTag
{
  WORK_TAG,
  DONE_TAG,
  HEARTBEAT,
  CHECKPOINT,
  CHECKPOINT_DONE
} MWTag;

typedef enum MASC {
	Preempted = -1,
	Success,
	Failure
} MW_API_STATUS_CODE;

#endif
