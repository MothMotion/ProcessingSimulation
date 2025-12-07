#ifndef POIS_DISTR_H
#define POIS_DISTR_H

#include "distr.h"
#include <cmath>
#include <complex>

#include <boost/math/distributions/poisson.hpp>

namespace Distribution {

typedef class PoisDistribution : public BaseDistribution {
private:
  float _param;
  boost::math::poisson_distribution<float> _distribution;
public:
  PoisDistribution(const float& margin, const float& param);
} pois_dt;

}

#endif
