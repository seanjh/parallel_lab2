#include <iostream>
#include <list>
#include <gmpxx.h>

class DivisorGenerator {

private:
	std::list<mpz_class> divisors;
	std::string divisorsCSV;
	const mpz_class dividend;
	const mpz_class firstValueToTest;
	const int count;
	void computeDivisors();
	void computeDivisorsCSV();

public:
	DivisorGenerator(mpz_class d, mpz_class f, int c);
	DivisorGenerator(std::string d, std::string f, int c);
	const std::list<mpz_class> &getDivisors();
	const std::string &getDivisorsCSV();
	std::list<mpz_class>::const_iterator getIterator();
};