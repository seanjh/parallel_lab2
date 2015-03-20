#ifndef __MW__DIVISOR_APPLICATION__
#define __MW__DIVISOR_APPLICATION__

#include <list>
#include "MW_API.hpp"
#include "DivisorResult.hpp"
#include "DivisorWork.hpp"
#include <string>
// #include <gmpxx.h>

class DivisorApplication : public MW_API {
public:
	DivisorApplication(std::string &divisorString);
	DivisorApplication(std::string &divisorString, mpz_class work_size);
	DivisorApplication(std::string &, std::string &);
	virtual std::list<std::shared_ptr<Work>> &work();
  virtual int results(std::list<Result *> *);

private:
	std::list<std::shared_ptr<Work>> workList;
	mpz_class divisor;
	mpz_class work_size;
};

#endif
