

#include <string>
#include <iostream>
#include "DivisorApplication.hpp"

DivisorApplication::DivisorApplication(std::string &divisorString) : divisor(mpz_class(divisorString, 10))
{
	
}

std::list<Work *> *DivisorApplication::work()
{
	std::list<Work *> *retList = new std::list<Work *>();

	mpz_class square_a = sqrt(divisor);
	std::string beginString1 = mpz_class(1).get_str();
	const int count1 = mpz_class(((square_a/2)-1)+1).get_si();
	DivisorWork *divWork1 = new DivisorWork(divisor.get_str(), beginString1, count1);
	retList->push_back(divWork1);

	std::string beginString2 = mpz_class(square_a/2+1).get_str();
	const int count2 = mpz_class(((square_a)-(square_a/2+1))+1).get_si();
	DivisorWork *divWork2 = new DivisorWork(divisor.get_str(), beginString2, count2);
	retList->push_back(divWork2);

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

	std::cout << divisor << " has " << results.size() << " total divisors.\n";
	for(auto iter = results.begin(); 
	    iter != results.end();
	    iter++) {
	std::cout << *iter << " is a divisor\n";
	}

	return 0;
}