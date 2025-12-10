#include "distr.h"
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <utility>

#ifdef DEBUG
#include <chrono>
#include <iostream>
#include <thread>
#endif

#define middle(x,y) (x+y)/2

namespace Distribution {

void Scope::set_value(const float& val, const scope_e& wh) {
  switch(wh) {
    case scope_e::start : start = val; break;
    case scope_e::end   : end   = val; break;
  }
  if(start > end)
    std::swap(start, end);
}

BaseDistribution::BaseDistribution(const scope_t& scope, const float& margin) {
  _scope = scope;
  _margin = margin;
}

//const float BaseDistribution::probability(const float& inp);

const float BaseDistribution::probability(const float& inp) {
  return inp;
}

const float BaseDistribution::inverse(const float& inp) {
  scope_t scope = _scope;

  if(scope.start == -INFINITY) 
    scope.start = -FLT_MAX;

  if(scope.end == INFINITY)
    scope.end = FLT_MAX;

  #ifdef DEBUG
  std::cout << "initscope: " << scope.start << " " << scope.end << "\n";
  #endif

  float mid_prob = probability(middle(scope.start, scope.end));
  while(fabsf( mid_prob - inp ) > _margin) {
    if(mid_prob < inp)
      scope.start = middle(scope.start, scope.end);
    else
      scope.end = middle(scope.start, scope.end);

    if(mid_prob != 1 && mid_prob == probability(middle(scope.start, scope.end)))
      return middle(scope.start, scope.end);
    mid_prob = probability(middle(scope.start, scope.end));
  }
  #ifdef DEBUG
  std::cout << "Returning value: " << middle(scope.start, scope.end) << "\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  #endif
  return middle(scope.start, scope.end);
}

const float BaseDistribution::get_random() {
  return inverse((float)rand()/RAND_MAX);
}

const scope_t& BaseDistribution::get_scope() {
  return _scope;
}

} 

#undef middle
