#include "MW_Timer.hpp"

MW_Timer::MW_Timer(double periodInSeconds): period(periodInSeconds)
{
	reset();
}

bool MW_Timer::get() const
{
	b_mutex.lock();
	bool retVal = MPI::Wtime() > expirationTime;
	b_mutex.unlock();
	
	return retVal;
}

void MW_Timer::reset()
{
	b_mutex.lock();
	expirationTime = MPI::Wtime() + period;
	b_mutex.unlock();
}
