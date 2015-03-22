#ifndef __MW__MW_API_TYPES__
#define __MW__MW_API_TYPES__

using namespace std;

// #define MAX_MESSAGE_SIZE 8092

typedef enum _MwTag
{
  WORK_TAG,
  DONE_TAG,
  HEARTBEAT_TAG,
  CHECKPOINT_TAG,
  CHECKPOINT_DONE_TAG,

  NOTHING_TAG=-1
} MWTag;

typedef enum MASC {
	Preempted = -1,
	Success,
	Failure
} MW_API_STATUS_CODE;

typedef unsigned long MW_ID;
const int     MAX_MESSAGE_SIZE            = 8092;
const double  HEARTBEAT_PERIOD            = 2;
const double  CHECKPOINT_PERIOD           = 60.0;
const bool    WORKER_FAIL_TEST_ON         = true;
const double  WORKER_FAILURE_PROBABILITY  = 0.25;
const bool    MASTER_FAIL_TEST_ON         = false;
const double  MASTER_FAILURE_PROBABILITY  = 0.000001;

#endif
