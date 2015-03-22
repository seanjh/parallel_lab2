#ifndef __MW__RANDOM__
#define __MW__RANDOM__

#include <random>
#include <chrono>
#include <iostream>

using namespace std;

const double DEFAULT_FAILURE_PROBABILITY = 0.01;

class MW_Random {
public:
  MW_Random(double _probability, int id, int size) : probability(_probability)
  {
    unsigned current = chrono::high_resolution_clock::now().time_since_epoch().count();
    current += id * size;
    dre = default_random_engine(current);
    distribution = uniform_real_distribution<double> (0.0, 1.0);
  }

  MW_Random(int id, int size)
  {
    MW_Random(DEFAULT_FAILURE_PROBABILITY, id, size);
  }

  bool random_fail()
  {
    double val = distribution(dre);
    // cout << " Rolled " << val << " against probability " << probability << endl;
    return val < probability;
  }
private:
  double probability;
  uniform_real_distribution<double> distribution;
  default_random_engine dre;
};

#endif // __MW__RANDOM__
