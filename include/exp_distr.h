#ifndef EXP_DISTR_H
#define EXP_DISTR_H

#include "distr.h"
#include <cmath>
#include <complex>

#include <boost/math/distributions/exponential.hpp>

namespace Distribution {

typedef class ExpDistribution : public BaseDistribution {
private:
  float _param;
  boost::math::exponential_distribution<float> _distribution;
public:
  ExpDistribution(const float& margin, const float& param);
} exp_dt;

}

#endif
