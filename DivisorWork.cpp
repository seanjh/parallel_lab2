#include "DivisorWork.hpp"
#include <assert.h>
#include <sstream>

DivisorWork::DivisorWork(mpz_class d, mpz_class f, mpz_class c):
		dividend(d),
		firstValueToTest(f),
		count(c),
		divisorResult(NULL),
		iterator(0)
{
	assert(count > 0);
}

DivisorWork::DivisorWork(std::string d, std::string f, std::string c):
	DivisorWork(mpz_class(d, 10), mpz_class(f, 10), mpz_class(c, 10)) {}

DivisorWork::DivisorWork(std::string serialObject)
{
	std::string divString;
	std::string firstValString;
	std::string countString;
	std::istringstream iss(serialObject);
	std::getline(iss,divString,',');
	std::getline(iss,firstValString,',');
	std::getline(iss,countString);

	dividend = mpz_class(divString, 10);
	firstValueToTest = mpz_class(firstValString, 10);
	count = mpz_class(countString, 10);
}

DivisorWork::~DivisorWork()
{
	delete divisorResult;
}

MW_API_STATUS_CODE DivisorWork::compute(const MW_Semaphore &preemptionSemaphore)
{
	// std::list<mpz_class> divisors;
	if(divisorResult == NULL)
	{
		for(; iterator<count; iterator++)
		{
			if(preemptionSemaphore.get() == true)
			{
				return Preempted;
			}
			mpz_class ithValue = firstValueToTest + iterator;
			// std::cout<<"Testing " << ithValue << std::endl;
			if(dividend % ithValue == 0)
			{
				tmpDivisors.push_back(ithValue);
				// std::cout<<"Adding " << ithValue << std::endl;
				mpz_class other = dividend/ithValue;
				if (ithValue != other)
				{
					tmpDivisors.push_back(other);
					// std::cout<<"Adding " << other << std::endl;
				}
			}
		}
		tmpDivisors.sort();
		divisorResult = new DivisorResult(tmpDivisors);
		return Success;
	}
	else
		return Success;
}

DivisorResult *DivisorWork::result()
{
	return divisorResult;
}

std::string *DivisorWork::serialize()
{
	return new std::string(dividend.get_str() + ',' +
						firstValueToTest.get_str() + ',' +
						count.get_str());
}
