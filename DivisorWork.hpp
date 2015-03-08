#ifndef __MW__DIVISOR_WORK__
#define __MW__DIVISOR_WORK__

#include <string>
#include "DivisorResult.hpp"
#include <iostream>
#include <list>
#include <gmpxx.h>
#include "Work.hpp"

class DivisorResult;
    
class DivisorWork : public Work {
public:
    DivisorWork(mpz_class d, mpz_class f, int c);
	DivisorWork(std::string d, std::string f, int c);
	DivisorWork(std::string serialObject);
	//const std::list<mpz_class> &getDivisors();
	//const std::string &getDivisorsCSV();
	//std::list<mpz_class>::const_iterator getIterator();

//Work Interface
public:
    // Result *compute();
    virtual DivisorResult *compute();
    virtual std::string *serialize();
    static DivisorWork *deserialize(const std::string &serialObject){return new DivisorWork(serialObject);}

private:
 //    std::list<mpz_class> divisors;
	// std::string divisorsCSV;
	mpz_class dividend;
	mpz_class firstValueToTest;
	int count;
	// void computeDivisors();
	// void computeDivisorsCSV();
};


#endif /* defined(__MW__DIVISOR_WORK__) */