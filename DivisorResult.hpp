#ifndef __MW__DIVISOR_RESULT__
#define __MW__DIVISOR_RESULT__

#include <string>
#include <iostream>
#include <list>
#include <gmpxx.h>

#include "Result.hpp"
//#include "DivisorWork.hpp"

using namespace std;

class DivisorResult : public Result {

public:
    // Must be present, even if it does nothing.

    //This needs to destroy all of its children to prevent memory leaks
    // virtual ~Result();

	//DivisorResult();
	DivisorResult(const list<mpz_class> &);
	DivisorResult(string);
	virtual DivisorResult* clone() const;
	// DivisorResult(const string &);

public:
	//void addDivisor(mpz_class);
    // string *serialize();
    // static DivisorResult *deserialize(const string &);
    // You could add more to the public interface, if you wanted to.
	//const string &getDivisorsCSV();
    virtual shared_ptr<string> serialize();
    static shared_ptr<Result> deserialize(const string &serialObject)
		{
			return shared_ptr<Result>(make_shared<DivisorResult>(serialObject));
		}
    const list<mpz_class> &getDivisors();
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
	list<mpz_class> divisors;

};

#endif
