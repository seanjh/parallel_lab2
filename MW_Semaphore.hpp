#ifndef __MW__SEMAPHORE__
#define __MW__SEMAPHORE__

#include <mutex>

class MW_Semaphore {
public:
	MW_Semaphore();
	bool get() const;
	void set();
	void clear();

private:
	mutable std::mutex b_mutex;
	bool value;
};

#endif //__MW__SEMAPHORE__