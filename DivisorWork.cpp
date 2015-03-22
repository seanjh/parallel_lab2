#include <assert.h>
#include <sstream>

#include "DivisorWork.hpp"

using namespace std;

DivisorWork::DivisorWork(mpz_class d, mpz_class f, mpz_class c):
		dividend(d),
		firstValueToTest(f),
		count(c),
		divisorResult(nullptr),
		iterator(0)
{
	assert(count > 0);
}

DivisorWork::DivisorWork(string d, string f, string c):
	DivisorWork(mpz_class(d, 10), mpz_class(f, 10), mpz_class(c, 10)) {}

DivisorWork::DivisorWork(string serialObject)
{
	string divString;
	string firstValString;
	string countString;
	istringstream iss(serialObject);
	getline(iss,divString,',');
	getline(iss,firstValString,',');
	getline(iss,countString);

	dividend = mpz_class(divString, 10);
	firstValueToTest = mpz_class(firstValString, 10);
	count = mpz_class(countString, 10);
}

DivisorWork::~DivisorWork()
{
	// delete divisorResult;
}

MW_API_STATUS_CODE DivisorWork::compute(const MW_Object &preemptionObject)
{
	// list<mpz_class> divisors;
	// cout << "In Compute" <<endl;
	// cout << divisorResult <<endl;

	if(!divisorResult)
	{
		// cout << "Computing results" <<endl;
		for(; iterator<count; iterator++)
		{
			if(preemptionObject.get() == true)
			{
				return Preempted;
			}
			mpz_class ithValue = firstValueToTest + iterator;
			// cout<<"Testing " << ithValue << endl;
			if(dividend % ithValue == 0)
			{
				tmpDivisors.push_back(ithValue);
				// cout<<"Adding " << ithValue << endl;
				mpz_class other = dividend/ithValue;
				if (ithValue != other)
				{
					tmpDivisors.push_back(other);
					// cout<<"Adding " << other << endl;
				}
			}
		}
		tmpDivisors.sort();
		// divisorResult = new DivisorResult(tmpDivisors);
		divisorResult = make_shared<DivisorResult> (DivisorResult(tmpDivisors));
		// cout << "Created Results" <<endl;
		// cout << *(divisorResult->serialize()) <<endl;
		return Success;
	}
	else
		return Success;
}

shared_ptr<Result> DivisorWork::result()
{
	assert(divisorResult);
	return dynamic_pointer_cast<Result>(divisorResult);
}

shared_ptr<string> DivisorWork::serialize()
{
	return make_shared<string>(dividend.get_str() + ',' +
						firstValueToTest.get_str() + ',' +
						count.get_str());
}
