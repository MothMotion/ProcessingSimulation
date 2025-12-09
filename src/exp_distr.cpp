#include "exp_distr.h"
#include "distr.h"
#include <cmath>
#include <iostream>

#include <boost/math/distributions/exponential.hpp>

namespace Distribution {

ExpDistribution::ExpDistribution(const float& margin, const float& param)
  : BaseDistribution(scope_t{0, INFINITY}, margin, [this](const float& val){ 
    return boost::math::cdf(this->_distribution, val); 
  }) {
  _distribution = boost::math::exponential_distribution<float>(param);
}

}
