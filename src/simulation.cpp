#include "simulation.h"

#ifdef DEBUG
#include <cstdio>
#include <iostream>
#endif

#define tmpl template<typename cl_t, typename tk_t>
#define sim_tmpl BaseSimulation<cl_t, tk_t>

bool task_t::operator<(const task_t& other) const {
  return timer < other.timer;
}

bool client_t::operator<(const client_t& other) const {
  return service_dur < other.service_dur;
}
