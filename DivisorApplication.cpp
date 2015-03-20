

#include <string>
#include <iostream>
#include "DivisorApplication.hpp"

DivisorApplication::DivisorApplication(std::string &divisorString, mpz_class ws) : divisor(mpz_class(divisorString, 10)), work_size(ws)
{

}

DivisorApplication::DivisorApplication(std::string &divisorString) : divisor(mpz_class(divisorString, 10)), work_size(100000)
{

}

DivisorApplication::DivisorApplication(std::string &divisorString, std::string &workSizeString) : divisor(mpz_class(divisorString, 10)), work_size(mpz_class(workSizeString, 10))
{

}

std::list<Work *> *DivisorApplication::work()
{
	std::list<Work *> *retList = new std::list<Work *>();
	mpz_class square_a = sqrt(divisor);

	// std::cout << square_a.get_str() << std::endl;

	mpz_class numberOfWork = ((square_a / work_size) + 1);
	mpz_class firstWorkSize = work_size;
	mpz_class lastWorkSize = square_a % work_size;
	if(lastWorkSize == 0)
		lastWorkSize = work_size;


	for(mpz_class i=0; i<numberOfWork-1; i++)
	{
		mpz_class beginVal = (i * work_size) + 1;
		// std::cout << i << ": " << beginVal.get_str() << ": " << work_size << std::endl;
		DivisorWork *divWork = new DivisorWork(divisor, beginVal, work_size);
		retList->push_back(divWork);
	}

	mpz_class beginVal = ((numberOfWork-1) * work_size) + 1;
	// std::cout << numberOfWork-1 << ": " << beginVal.get_str() << ": " << lastWorkSize << std::endl;
	DivisorWork *divWork = new DivisorWork(divisor, beginVal, lastWorkSize);
	retList->push_back(divWork);

	return retList;
}

int DivisorApplication::results(std::list<Result *> *listOfResults)
{
	std::list<mpz_class> results;

	for (	auto riter = listOfResults->begin();
			riter != listOfResults->end();
			riter++)
	{
		DivisorResult *divResult = dynamic_cast<DivisorResult *>(*riter);
		std::list<mpz_class> divisorList(divResult->getDivisors());
		results.merge(divisorList);
	}


	std::cout<< "Dividend: " <<divisor<<" Work Size: "<<work_size<<" Work Count: "<<mpz_class((sqrt(divisor) / work_size) + 1).get_str()<<" ";
	std::cout << divisor << " has " << results.size() << " total divisors.\n";
	for(auto iter = results.begin();
	    iter != results.end();
	    iter++) {
	std::cout << *iter << " is a divisor\n";
	}

	return 0;
}
