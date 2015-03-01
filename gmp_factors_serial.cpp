#include <iostream>
#include <list>
#include <gmpxx.h>
using namespace std;

int main(int argc, char* argv[])
{
  mpz_class a = mpz_class(argv[1], 10);

  list<mpz_class> results;

  mpz_class square_a = sqrt(a);
  string beginString = mpz_class(1).get_str();
  string endString = mpz_class(square_a).get_str();

  //cout<<"Begin String: "<<beginString<<endl;
  //cout<<"End String: "<<endString<<endl;

  mpz_class begin = mpz_class(beginString, 10);
  mpz_class end = mpz_class(endString, 10);

  for (mpz_class i = begin; i<=end; i++) {
    if (a % i == 0) {
      results.push_back(i);
      mpz_class other = a/i;
      if (i != other)
        results.push_back(other);
    }
  }

  results.sort();

  list<mpz_class>::iterator iter;
  cout << a << " has " << results.size() << " total divisors.\n";
  for(iter = results.begin(); 
        iter != results.end();
        iter++) {
    cout << *iter << " is a divisor\n";
  }
}
