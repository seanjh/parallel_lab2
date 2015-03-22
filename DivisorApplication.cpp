#include <string>
#include <iostream>
#include "DivisorApplication.hpp"
using namespace std;

DivisorApplication::DivisorApplication(string &divisorString, mpz_class ws) : divisor(mpz_class(divisorString, 10)), work_size(ws)
{

}

DivisorApplication::DivisorApplication(string &divisorString) : divisor(mpz_class(divisorString, 10)), work_size(100000)
{

}

DivisorApplication::DivisorApplication(string &divisorString, string &workSizeString) : divisor(mpz_class(divisorString, 10)), work_size(mpz_class(workSizeString, 10))
{

}

list<shared_ptr<Work>> &DivisorApplication::work()
{
	// list<shared_ptr<Work>> *retList = list<shared_ptr<Work>>;

	mpz_class square_a = sqrt(divisor);

	// cout << square_a.get_str() << endl;

	mpz_class numberOfWork = ((square_a / work_size) + 1);
	mpz_class firstWorkSize = work_size;
	mpz_class lastWorkSize = square_a % work_size;
	if(lastWorkSize == 0)
		lastWorkSize = work_size;



	for(mpz_class i=0; i<numberOfWork-1; i++)
	{
		mpz_class beginVal = (i * work_size) + 1;
		// cout << i << ": " << beginVal.get_str() << ": " << work_size << endl;
		// TODO shared ptr
		shared_ptr<DivisorWork> divWork(new DivisorWork(divisor, beginVal, work_size));
		// DivisorWork *divWork = new DivisorWork(divisor, beginVal, work_size);
		workList.push_back(divWork);
	}

	mpz_class beginVal = ((numberOfWork-1) * work_size) + 1;
	// cout << numberOfWork-1 << ": " << beginVal.get_str() << ": " << lastWorkSize << endl;

	// TODO shared ptr
	shared_ptr<DivisorWork> divWork(new DivisorWork(divisor, beginVal, lastWorkSize));
	// DivisorWork *divWork = new DivisorWork(divisor, beginVal, lastWorkSize);
	workList.push_back(divWork);

	return workList;
}

int DivisorApplication::results(shared_ptr<list<shared_ptr<Result>>> listOfResults)
{
	list<mpz_class> results;

	for (	auto riter = listOfResults->begin();
			riter != listOfResults->end();
			riter++)
	{
		shared_ptr<DivisorResult> divResult = dynamic_pointer_cast<DivisorResult>(*riter);
		list<mpz_class> divisorList(divResult->getDivisors());
		results.merge(divisorList);
	}


	cout<< "Dividend: " <<divisor<<" Work Size: "<<work_size<<" Work Count: "<<mpz_class((sqrt(divisor) / work_size) + 1).get_str()<<" ";
	cout << divisor << " has " << results.size() << " total divisors.\n";
	for(auto iter = results.begin();
	    iter != results.end();
	    iter++) {
	cout << *iter << " is a divisor\n";
	}

	return 0;
}
