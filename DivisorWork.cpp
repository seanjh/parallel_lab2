#include "DivisorWork.hpp"
#include <assert.h>
#include <sstream>

DivisorWork::DivisorWork(mpz_class d, mpz_class f, mpz_class c):
		dividend(d),
		firstValueToTest(f),
		count(c)
{
	assert(count > 0);
}

DivisorWork::DivisorWork(std::string d, std::string f, std::string c):
	DivisorWork(mpz_class(d, 10), mpz_class(f, 10), mpz_class(c, 10)) {}

DivisorWork::DivisorWork(std::string serialObject)
{
	std::cout << "DivisorWork::DivisorWork serialObject is " << serialObject << std::endl;
	std::string divString;
	std::string firstValString;
	std::string countString;
	std::istringstream iss(serialObject);
	std::getline(iss,divString,',');
	std::getline(iss,firstValString,',');
	std::getline(iss,countString);

	std::cout << "DivisorWork::DivisorWork divString is " << divString << std::endl;
	std::cout << "DivisorWork::DivisorWork firstValString is " << firstValString << std::endl;
	std::cout << "DivisorWork::DivisorWork countString is " << countString << std::endl;

	dividend = mpz_class(divString, 10);
	firstValueToTest = mpz_class(firstValString, 10);
	count = mpz_class(countString, 10);

	std::cout << "DivisorWork::DivisorWork divisor is " << dividend.get_str() << std::endl;
	std::cout << "DivisorWork::DivisorWork firstVal is " << firstValueToTest.get_str() << std::endl;
	std::cout << "DivisorWork::DivisorWork count is " << count.get_str() << std::endl;

}

// DivisorWork::~DivisorWork(){}

DivisorResult *DivisorWork::compute()
{
	std::list<mpz_class> divisors;
	for(mpz_class i=0; i<count; i++)
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
						count.get_str());
}
