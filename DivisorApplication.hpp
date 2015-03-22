#ifndef __MW__DIVISOR_APPLICATION__
#define __MW__DIVISOR_APPLICATION__

#include <list>
#include "MW_API.hpp"
#include "DivisorResult.hpp"
#include "DivisorWork.hpp"
#include <string>
using namespace std;
// #include <gmpxx.h>

class DivisorApplication : public MW_API {
public:
	DivisorApplication(string &divisorString);
	DivisorApplication(string &divisorString, mpz_class work_size);
	DivisorApplication(string &, string &);
	virtual list<shared_ptr<Work>> &work();
  virtual int results(shared_ptr<list<shared_ptr<Result>>>);

private:
	list<shared_ptr<Work>> workList;
	mpz_class divisor;
	mpz_class work_size;
};

#endif
