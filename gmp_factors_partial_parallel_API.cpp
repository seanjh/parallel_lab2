#include <iostream>
#include <list>
#include <gmpxx.h>
#include "DivisorWork.hpp"
#include "DivisorResult.hpp"

using namespace std;

list<mpz_class> csvToList(string csv)
{
  cout << csv << endl;
  list<mpz_class> retList;
  auto outIter=csv.begin();
  auto inIter=csv.begin();
  for(;outIter!=csv.end();outIter = inIter)
  {
    string number;
    //cerr << *inIter << endl;
    while(inIter != csv.end())
    {
      if(*inIter == ',')
      {
        inIter++;
        break;
      }
      // cerr << *inIter << endl;
      number += *inIter;
      inIter++;
    }
    // cerr << number << endl;
    // cerr << number.length() << endl;
    retList.push_back(mpz_class(number, 10));
  }
  return retList;
}

int main(int argc, char* argv[])
{
  mpz_class a = mpz_class(argv[1], 10);

  //list<mpz_class> results;

  mpz_class square_a = sqrt(a);
  string beginString1 = mpz_class(1).get_str();
  const int count1 = mpz_class(((square_a/2)-1)+1).get_si();
  // cout << "Creating DivisorWork"<<endl;
  DivisorWork *divWork1 = new DivisorWork(a.get_str(), beginString1, count1);
  // cout << *(divWork1->serialize()) <<endl;

  string beginString2 = mpz_class(square_a/2+1).get_str();
  const int count2 = mpz_class(((square_a)-(square_a/2+1))+1).get_si();
  DivisorWork *divWork2 = new DivisorWork(a.get_str(), beginString2, count2);
  // cout << *(divWork2->serialize()) <<endl;

  DivisorResult *divResult1 = divWork1->compute();
  DivisorResult *divResult2 = divWork2->compute();

  // cout << *(divResult1->serialize())<<endl;
  // cout << *(divResult2->serialize())<<endl;




  //cout << divGen1.getDivisorsCSV()<<endl;
  // list<mpz_class> divs1FromCSV = csvToList(*(divResult1->serialize()));
  // list<mpz_class>::iterator iter;
  // for(iter = divs1FromCSV.begin(); 
  //       iter != divs1FromCSV.end();
  //       iter++) {
  //   cout << *iter << " is a divisor\n";
  // }
  // cout << divResult2->serialize()<<endl;

  list<mpz_class> results;
  list<mpz_class> divs1(divResult1->getDivisors());
  list<mpz_class> divs2(divResult2->getDivisors());
  results.merge(divs1);
  results.merge(divs2);

  cout << a << " has " << results.size() << " total divisors.\n";
  for(auto iter = results.begin(); 
        iter != results.end();
        iter++) {
    cout << *iter << " is a divisor\n";
  }
}
