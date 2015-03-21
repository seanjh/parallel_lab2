#include "DivisorWork.hpp"
#include <assert.h>
#include <sstream>

DivisorWork::DivisorWork(mpz_class d, mpz_class f, mpz_class c):
		dividend(d),
		firstValueToTest(f),
		count(c),
		divisorResult(nullptr),
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
	// delete divisorResult;
}

MW_API_STATUS_CODE DivisorWork::compute(const MW_Object &preemptionObject)
{
	// std::list<mpz_class> divisors;
	// std::cout << "In Compute" <<std::endl;
	// std::cout << divisorResult <<std::endl;

	if(!divisorResult)
	{
		// std::cout << "Computing results" <<std::endl;
		for(; iterator<count; iterator++)
		{
			if(preemptionObject.get() == true)
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
		// divisorResult = new DivisorResult(tmpDivisors);
		divisorResult = std::make_shared<DivisorResult> (DivisorResult(tmpDivisors));
		// std::cout << "Created Results" <<std::endl;
		// std::cout << *(divisorResult->serialize()) <<std::endl;
		return Success;
	}
	else
		return Success;
}

std::shared_ptr<Result> DivisorWork::result()
{
	assert(divisorResult);
	return std::dynamic_pointer_cast<Result>(divisorResult);
}

std::string *DivisorWork::serialize()
{
	return new std::string(dividend.get_str() + ',' +
						firstValueToTest.get_str() + ',' +
						count.get_str());
}
