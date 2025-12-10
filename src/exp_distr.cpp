#include "exp_distr.h"
#include "distr.h"
#include <cmath>
#include <iostream>

#include <boost/math/distributions/exponential.hpp>

namespace Distribution {

ExpDistribution::ExpDistribution(const float& margin, const float& param)
  : BaseDistribution(scope_t{0, INFINITY}, margin), _param(param),
  _distribution(boost::math::exponential_distribution<float>(param)) {
  #ifdef DEBUG
  std::cout << "Param: " << this << "\n";
  #endif
}

const float exp_dt::probability(const float& inp) {
  return boost::math::cdf(_distribution, inp);
}

}
