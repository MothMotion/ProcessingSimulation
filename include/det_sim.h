#ifndef DETAILED_SIMULATION_H
#define DETAILED_SIMULATION_H

#include "simulation.hpp"
#include <cstdint>

typedef struct DetailedMetric {
  float t_in_queue = 0;
  float t_in_service = 0;
  float a_queue_size = 0;
  float a_service_size = 0;
} dmetric_t;

typedef struct DetailedClient : public Client {
  float in_service = 0;
  float in_queue = 0;
} dclient_t;

extern template class BaseSimulation<dclient_t, task_t>;

class DetailedSimulation : public BaseSimulation<dclient_t, task_t> { 
protected:
  dmetric_t _dmetric;
private:
public:
  DetailedSimulation(const uint16_t& queue_limit = 0, const uint16_t& service_limit = 0,
                     const float& margin = 0.0f) : BaseSimulation<dclient_t, task_t>(queue_limit, service_limit, margin) {}

  void simulate(float simulate_time,
                const std::function<void()>& setup_function,
                const std::function<void()>& after_service);

  dmetric_t& get_d_metric();

  inline const size_t get_queue_size() const {return _client_queue.size();}
  inline const size_t get_service_size() const {return _service_queue.size();}
};

typedef DetailedSimulation dsim_t;

#endif
