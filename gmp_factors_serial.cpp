#include <iostream>
#include <list>
#include <stdlib.h>
#include <gmpxx.h>
using namespace std;

int main(int argc, char* argv[])
{
  mpz_class a = atoi(argv[1]);

  list<mpz_class> results;

  mpz_class square_a = sqrt(a) + 1;
  mpz_class i;
  for (i=2; i<square_a; i++) {
    if (a % i == 0) {
      cout << i << " is a divisor\n";
      results.push_back(i);
      if (i != square_a) {
        cout << a / i << " is a divisor\n";
        results.push_back(a / i);
      }
    }
  }

  results.sort();
  cout << a << " has " << results.size() << " total divisors.\n";
}
