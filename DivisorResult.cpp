#include "DivisorResult.hpp"
#include <sstream>


DivisorResult::DivisorResult(const std::list<mpz_class> &divList)
{
	divisors = divList;
}

// DivisorResult::DivisorResult(const std::string &serialObject)
DivisorResult::DivisorResult(std::string serialObject)
{
	std::cout << "DivisorResult::DivisorResult serialObject is " << serialObject << std::endl;
	std::istringstream iss (serialObject);
	std::string divString;

	std::cout << "DivisorResult::DivisorResult divString is " << divString << std::endl;

	while(std::getline(iss,divString,','))
    {
			std::cout << "DivisorResult::DivisorResult pushing substring " << divString << std::endl;
      divisors.push_back(mpz_class(divString, 10));
    }
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
