#ifndef __MW__DIVISOR_WORK__
#define __MW__DIVISOR_WORK__

#include <string>
#include <iostream>
#include <list>
#include <gmpxx.h>

#include "Work.hpp"
#include "DivisorResult.hpp"
#include "MW_Object.hpp"
#include "MW_API_Types.hpp"

using namespace std;

class DivisorResult;

class DivisorWork : public Work {
public:
  DivisorWork(mpz_class d, mpz_class f, mpz_class c);
  DivisorWork(string d, string f, string c);
  DivisorWork(string serialObject);
  ~DivisorWork();
	//const list<mpz_class> &getDivisors();
	//const string &getDivisorsCSV();
	//list<mpz_class>::const_iterator getIterator();

//Work Interface
public:
  // Result *compute();
  virtual MW_API_STATUS_CODE compute(const MW_Object &);
  virtual shared_ptr<Result> result();
  virtual shared_ptr<string> serialize();
  static  shared_ptr<Work> deserialize(const string &serialObject)
  {
    return shared_ptr<Work>(make_shared<DivisorWork>(serialObject));
  }

private:
 //    list<mpz_class> divisors;
	// string divisorsCSV;
	mpz_class dividend;
	mpz_class firstValueToTest;
	mpz_class count;
	list<mpz_class> tmpDivisors;
	shared_ptr<DivisorResult> divisorResult;
	mpz_class iterator;
	// void computeDivisors();
	// void computeDivisorsCSV();
};


#endif /* defined(__MW__DIVISOR_WORK__) */
