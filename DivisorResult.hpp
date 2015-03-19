#ifndef __MW__DIVISOR_RESULT__
#define __MW__DIVISOR_RESULT__

#include <string>
//#include "DivisorWork.hpp"
#include <iostream>
#include <list>
#include <gmpxx.h>
#include "Result.hpp"

class DivisorResult : public Result {

public:
    // Must be present, even if it does nothing.

    //This needs to destroy all of its children to prevent memory leaks
    // virtual ~Result();

	//DivisorResult();
	DivisorResult(const std::list<mpz_class> &);
	DivisorResult(std::string);
    virtual DivisorResult* clone() const;
	// DivisorResult(const std::string &);

public:
	//void addDivisor(mpz_class);
    // std::string *serialize();
    // static DivisorResult *deserialize(const std::string &);
    // You could add more to the public interface, if you wanted to.
	//const std::string &getDivisorsCSV();
    virtual std::string *serialize();
    static DivisorResult *deserialize(const std::string &serialObject){return new DivisorResult(serialObject);}
    const std::list<mpz_class> &getDivisors();
private:
    // Private things for your implementation.  Probably will not need
    // anything, since this primarily an interface class.
	std::list<mpz_class> divisors;


};

#endif
