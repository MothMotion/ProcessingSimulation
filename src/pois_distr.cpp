#include "pois_distr.h"
#include "distr.h"
#include <cmath>

#include <boost/math/distributions/poisson.hpp>

namespace Distribution {

PoisDistribution::PoisDistribution(const float& margin, const float& param)
  : BaseDistribution(scope_t{0, 10000}, margin), _param(param),
  _distribution(boost::math::poisson_distribution<float>(param)) {}

const float pois_dt::probability(const float& inp) {
  return boost::math::cdf(_distribution, inp);
}

}
