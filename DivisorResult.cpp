#include "DivisorResult.hpp"
#include <sstream>


DivisorResult::DivisorResult(const std::list<mpz_class> &divList)
{
	divisors = divList;
}

DivisorResult::DivisorResult(std::string serialObject)
{
	std::istringstream iss (serialObject);
	std::string divString;

	//std::cout << "Serialized result object: " <<serialObject<<std::endl;
	while(std::getline(iss,divString,','))
    {
        divisors.push_back(mpz_class(divString, 10));
    }
}

DivisorResult* DivisorResult::clone() const
{
	return new DivisorResult(*this);
}

std::string *DivisorResult::serialize()
{
	std::string divisorsString;
	std::list<mpz_class>::const_iterator divIter = divisors.cbegin();
	if(divIter!=divisors.cend())
	{
		divisorsString += divIter->get_str();
		divIter++;
	}

	for(;divIter!=divisors.cend();divIter++)
	{
		divisorsString += "," + divIter->get_str();
	}

	return new std::string(divisorsString);
}

const std::list<mpz_class> &DivisorResult::getDivisors()
{
	return const_cast<std::list<mpz_class> &>(divisors);
}
