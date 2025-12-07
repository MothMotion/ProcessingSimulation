#include "pois_distr.h"
#include "distr.h"
#include <cmath>

#include <boost/math/distributions/poisson.hpp>

namespace Distribution {

PoisDistribution::PoisDistribution(const float& margin, const float& param)
  : BaseDistribution(scope_t{0, 10000}, margin, [this](const float& val){
    return boost::math::cdf(this->_distribution, val); 
  }) {
  _distribution = boost::math::poisson_distribution<float>(param);
}

}
