#ifndef __MW__SEMAPHORE__
#define __MW__SEMAPHORE__

#include <mutex>
#include "MW_Object.hpp"

class MW_Semaphore : public MW_Object {
public:
	MW_Semaphore();
	virtual bool get() const;
	virtual void set();
	virtual void clear();

protected:
	mutable std::mutex b_mutex;
	bool value;
};

#endif //__MW__SEMAPHORE__