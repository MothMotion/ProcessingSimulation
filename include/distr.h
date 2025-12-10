#ifndef DISTR_H
#define DISTR_H

#include <cmath>
#include <functional>

namespace Distribution {

typedef struct Scope {
  float start;
  float end;

  typedef enum class EScopeEnd : bool {
    start = 0,
    end = 1
  } scope_e;

  void set_value(const float& inp, const scope_e& which_end);
} scope_t;

class BaseDistribution {
protected:
  scope_t _scope;
  float _margin;
  std::function<float(const float&)> _function;

public:
  BaseDistribution(const scope_t& scope = {0, 1},
                   const float& margin = 0);

  virtual const float probability(const float& input);
  const float inverse(const float& input);

  const scope_t& get_scope();
  const float get_random();
};

}

#endif
