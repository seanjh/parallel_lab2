#include "DivisorWork.hpp"
#include <assert.h>
#include <sstream>

DivisorWork::DivisorWork(mpz_class d, mpz_class f, int c):
		dividend(d),
		firstValueToTest(f),
		count(c)
{
	assert(count > 0);
}

DivisorWork::DivisorWork(std::string d, std::string f, int c):
	DivisorWork(mpz_class(d, 10), mpz_class(f, 10), c) {}

DivisorWork::DivisorWork(std::string serialObject)
{
	std::cout << "DivisorWork::DivisorWork serialObject is " << serialObject << std::endl;
	std::string divString;
	std::string firstValString;
	std::string countString;
	int count;
	std::istringstream iss(serialObject);
	std::getline(iss,divString,',');
	std::getline(iss,firstValString,',');
	std::getline(iss,countString);

	std::cout << "DivisorWork::DivisorWork divString is " << divString << std::endl;
	std::cout << "DivisorWork::DivisorWork firstValString is " << firstValString << std::endl;
	std::cout << "DivisorWork::DivisorWork countString is " << countString << std::endl;

	dividend = mpz_class(divString, 10);
	firstValueToTest = mpz_class(firstValString, 10);
	count = stoi(countString);
}

// DivisorWork::~DivisorWork(){}

DivisorResult *DivisorWork::compute()
{
	std::list<mpz_class> divisors;
	for(int i=0; i<count; i++)
	{
		mpz_class ithValue = firstValueToTest + i;
		// std::cout<<"Testing " << ithValue << std::endl;
		if(dividend % ithValue == 0)
		{
			divisors.push_back(ithValue);
			// std::cout<<"Adding " << ithValue << std::endl;
			mpz_class other = dividend/ithValue;
			if (ithValue != other)
			{
				divisors.push_back(other);
				// std::cout<<"Adding " << other << std::endl;
			}
		}
	}
	divisors.sort();
	return new DivisorResult(divisors);
}

std::string *DivisorWork::serialize()
{
	return new std::string(dividend.get_str() + ',' +
						firstValueToTest.get_str() + ',' +
						std::to_string(count));
}