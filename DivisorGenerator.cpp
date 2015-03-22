#include "DivisorGenerator.hpp"
#include <assert.h>
#include <iostream>
using namespace std;

DivisorGenerator::DivisorGenerator(mpz_class d, mpz_class f, int c):
		dividend(d),
		firstValueToTest(f),
		count(c)
{
	assert(count > 0);

	computeDivisors();
}

DivisorGenerator::DivisorGenerator(string d, string f, int c):
	DivisorGenerator(mpz_class(d, 10), mpz_class(f, 10), c) {}

// DivisorGenerator::DivisorGenerator(string input):
// 	DivisorGenerator(mpz_class(d, 10), mpz_class(f, 10), c)
// {


// }

void DivisorGenerator::computeDivisors()
{
	for(int i=0; i<count; i++)
	{
		mpz_class ithValue = firstValueToTest + i;
		// cout<<"Testing " << ithValue << endl;
		if(dividend % ithValue == 0)
		{
			divisors.push_back(ithValue);
			// cout<<"Adding " << ithValue << endl;
			mpz_class other = dividend/ithValue;
			if (ithValue != other)
			{
				divisors.push_back(other);
				// cout<<"Adding " << other << endl;
			}
		}
	}
	divisors.sort();
	computeDivisorsCSV();
}

const list<mpz_class> &DivisorGenerator::getDivisors()
{
	return divisors;
}

const string &DivisorGenerator::getDivisorsCSV()
{
	return divisorsCSV;
}

void DivisorGenerator::computeDivisorsCSV()
{
	string divisorsString;
	list<mpz_class>::const_iterator divIter = divisors.cbegin();
	if(divIter!=divisors.cend())
	{
		divisorsString += divIter->get_str();
		// cout<<divIter->get_str()<<endl;
		// cout<<divisorsString<<endl;
		divIter++;
	}

	for(;divIter!=divisors.cend();divIter++)
	{
		divisorsString += "," + divIter->get_str();
		// cout<<divIter->get_str()<<endl;
		// cout<<divisorsString<<endl;
	}

	divisorsCSV = string(divisorsString);
}

list<mpz_class>::const_iterator DivisorGenerator::getIterator()
{
	return divisors.cbegin();
}
