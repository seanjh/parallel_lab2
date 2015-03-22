#include <sstream>

#include "DivisorResult.hpp"

using namespace std;


DivisorResult::DivisorResult(const list<mpz_class> &divList)
{
	divisors = divList;
}

DivisorResult::DivisorResult(string serialObject)
{
	istringstream iss (serialObject);
	string divString;

	//cout << "Serialized result object: " <<serialObject<<endl;
	while(getline(iss,divString,','))
    {
        divisors.push_back(mpz_class(divString, 10));
    }
}

DivisorResult* DivisorResult::clone() const
{
	return new DivisorResult(*this);
}

shared_ptr<string> DivisorResult::serialize()
{
	string divisorsString;
	list<mpz_class>::const_iterator divIter = divisors.cbegin();
	if(divIter!=divisors.cend())
	{
		divisorsString += divIter->get_str();
		divIter++;
	}

	for(;divIter!=divisors.cend();divIter++)
	{
		divisorsString += "," + divIter->get_str();
	}

	return make_shared<string>(divisorsString);
}

const list<mpz_class> &DivisorResult::getDivisors()
{
	return const_cast<list<mpz_class> &>(divisors);
}
