#include "DivisorWork.hpp"
#include <assert.h>
#include <sstream>

DivisorWork::DivisorWork(mpz_class d, mpz_class f, mpz_class c): 
		dividend(d),
		firstValueToTest(f),
		count(c)
{
	// std::cout<<"In divisor work constructor: " <<d.get_str() << \
	// " : "<<f.get_str() << " : "<<c.get_str() << std::endl;
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

DivisorWork::~DivisorWork(){}

DivisorResult *DivisorWork::compute()
{

	//std::cout << "In Compute" << std::endl;
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
	//std::cout << "completed Compute" << std::endl;
	return new DivisorResult(divisors);
}

std::string *DivisorWork::serialize()
{
	std::string *retString = new std::string(dividend.get_str() + ',' + 
						firstValueToTest.get_str() + ',' + 
						count.get_str());
	// std::cout <<dividend.get_str() + ',' + 
	// 					firstValueToTest.get_str() + ',' + 
	// 					count.get_str() << std::endl;
	//std::cout << *retString << std::endl;
	return retString;
	// std::cout << dividend.get_str() << std::endl;
	// std::cout << firstValueToTest.get_str() << std::endl;
	// std::cout << count.get_str() << std::endl;
	// return NULL;				
}
