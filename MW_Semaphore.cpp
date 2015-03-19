#include "MW_Semaphore.hpp"

MW_Semaphore::MW_Semaphore(): value(false)
{

}

bool MW_Semaphore::get() const
{
	b_mutex.lock();
	bool retVal = value;
	b_mutex.unlock();
	return retVal;
}

void MW_Semaphore::set()
{
	b_mutex.lock();
	value = true;
	b_mutex.unlock();
}

void MW_Semaphore::clear()
{
	b_mutex.lock();
	value = false;
	b_mutex.unlock();
}