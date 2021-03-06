#ifndef __MW__MW_API_TYPES__
#define __MW__MW_API_TYPES__

#define MAX_MESSAGE_SIZE 8092

typedef enum _MwTag
{
  WORK_TAG,
  DONE_TAG,
  HEARTBEAT_TAG,
  CHECKPOINT_TAG,
  NEW_MASTER_TAG,
  NOTHING_TAG=-1
} MWTag;

typedef enum MASC {
	Preempted = -1,
	Success,
	Failure
} MW_API_STATUS_CODE;

typedef unsigned long MW_ID;
const double  PREEMPTION_TIMER_PERIOD     = 0.1;
const double  HEARTBEAT_PERIOD            = 1.0;
const double  CHECKPOINT_PERIOD           = 20.0;
const bool    WORKER_FAIL_TEST_ON         = true;
const double  WORKER_FAILURE_PROBABILITY  = 0.001;
const bool    MASTER_FAIL_TEST_ON         = true;
const double  MASTER_FAILURE_PROBABILITY  = 0.01;
const double  SEND_FAILURE_PROBABILITY    = 0.01;
const double STARTUP_DELAY_TIME = 10.0;

#endif
