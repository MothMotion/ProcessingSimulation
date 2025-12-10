#ifndef DETAILED_DIFFERENTED_SIMULATION_H
#define DETAILED_DIFFERENTED_SIMULATION_H

#include "det_sim.h"
#include "simulation.h"
#include <cstdint>

typedef struct DetailedDifferentedClient : public DetailedClient {
  float* arr_service_dur = nullptr;

  //~DetailedDifferentedClient() {delete arr_service_dur;} double free, lol
} ddclient_t;

typedef struct DetailedDifferentedMetric : public DetailedMetric {
  float* arr_status = nullptr;

} ddmetric_t;

class DetailedDifferentedSimulation : public BaseSimulation<ddclient_t> { 
protected:
  ddmetric_t _dmetric;
private:
public:
  DetailedDifferentedSimulation(const uint16_t& queue_limit = 0,
                                const uint16_t& service_limit = 0,
                                const float& margin = 0.0f)
    : BaseSimulation(queue_limit, service_limit, margin) {
    _service_queue.resize(service_limit);
  }

  void simulate(float simulate_time,
                const std::function<void()>& setup_function,
                const std::function<void()>& after_service);

  void add_client(const ddclient_t& client);

  const ddclient_t get_service() const;

  inline ddmetric_t& get_d_metric() {return _dmetric;}

  inline const size_t get_queue_size() const {return _client_queue.size();}
  const size_t get_service_size() const;
};

typedef DetailedDifferentedSimulation ddsim_t;

#endif
