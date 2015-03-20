#ifndef __MW__RANDOM__
#define __MW__RANDOM__

#include <random>
#include <iostream>

#define FAILURE_PROBABILITY 0.01

class MW_Random {
public:
  MW_Random(int id, int size)
  {
    unsigned current = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    current += id * size;
    dre = std::default_random_engine(current);
    distribution = std::uniform_real_distribution<double> (0.0, 1.0);
  }

  bool random_fail()
  {
    return distribution(dre) < FAILURE_PROBABILITY;
  }
private:
  std::uniform_real_distribution<double> distribution;
  std::default_random_engine dre;
};

#endif // __MW__RANDOM__
