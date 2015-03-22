#include <iostream>
#include <list>
#include <gmpxx.h>
using namespace std;

class DivisorGenerator {

private:
	list<mpz_class> divisors;
	string divisorsCSV;
	const mpz_class dividend;
	const mpz_class firstValueToTest;
	const int count;
	void computeDivisors();
	void computeDivisorsCSV();

public:
	DivisorGenerator(mpz_class d, mpz_class f, int c);
	DivisorGenerator(string d, string f, int c);
	const list<mpz_class> &getDivisors();
	const string &getDivisorsCSV();
	list<mpz_class>::const_iterator getIterator();
};
