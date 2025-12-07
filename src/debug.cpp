#include "pois_distr.h"
#include "main.h"

#include <iostream>

void debug() {
  Distribution::PoisDistribution distr = Distribution::PoisDistribution(0.001f, 1.0f);

  std::cout << "Probability Test: " << distr.probability(1.0f) << ". Expected ~0.7358\n";
  std::cout << "Inverse Test: " << distr.inverse(distr.probability(1.66f)) << ". Expected 1.66\n";
}
