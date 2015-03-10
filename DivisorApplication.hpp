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
	virtual std::list<Work *> *work();
  virtual int results(std::list<Result *> *);

	virtual std::string *workSerializer(Work &);
	virtual Work *workDeserializer(const std::string &);
	virtual std::string *resultSerializer(Result &);
	virtual Result *resultDeserializer(const std::string &);

private:
	mpz_class divisor;
};

#endif