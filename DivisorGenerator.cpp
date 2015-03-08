#include "DivisorGenerator.hpp"
#include <assert.h>
#include <iostream>

DivisorGenerator::DivisorGenerator(mpz_class d, mpz_class f, int c): 
		dividend(d),
		firstValueToTest(f),
		count(c)
{
	assert(count > 0);

	computeDivisors();
}

DivisorGenerator::DivisorGenerator(std::string d, std::string f, int c): 
	DivisorGenerator(mpz_class(d, 10), mpz_class(f, 10), c) {}

// DivisorGenerator::DivisorGenerator(std::string input): 
// 	DivisorGenerator(mpz_class(d, 10), mpz_class(f, 10), c) 
// {


// }

void DivisorGenerator::computeDivisors()
{
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
	computeDivisorsCSV();
}

const std::list<mpz_class> &DivisorGenerator::getDivisors()
{
	return divisors;
}

const std::string &DivisorGenerator::getDivisorsCSV()
{
	return divisorsCSV;
}

void DivisorGenerator::computeDivisorsCSV()
{
	std::string divisorsString;
	std::list<mpz_class>::const_iterator divIter = divisors.cbegin();
	if(divIter!=divisors.cend())
	{
		divisorsString += divIter->get_str();
		// std::cout<<divIter->get_str()<<std::endl;
		// std::cout<<divisorsString<<std::endl;
		divIter++;
	}

	for(;divIter!=divisors.cend();divIter++)
	{
		divisorsString += "," + divIter->get_str();
		// std::cout<<divIter->get_str()<<std::endl;
		// std::cout<<divisorsString<<std::endl;
	}

	divisorsCSV = std::string(divisorsString);
}

std::list<mpz_class>::const_iterator DivisorGenerator::getIterator()
{
	return divisors.cbegin();
}
