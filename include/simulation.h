#ifndef SIMULATION_H
#define SIMULATION_H

#include <cstdint>
#include <functional>
#include <string>
#include <deque>

typedef struct Client {
  float service_dur;

  bool operator<(const Client& other) const;
} client_t;

typedef struct Task {
  std::string name;
  float timer;
  std::function<void()> func;

  bool operator<(const Task& other) const;
} task_t;

typedef struct Metrics {
  uint16_t accept, serviced, deny;
} metric_t;

template<typename cl_t = client_t, typename tk_t = task_t>
class BaseSimulation {
private:
protected:
  std::deque<cl_t> _client_queue;
  std::deque<tk_t> _task_queue;
  std::deque<cl_t> _service_queue;
  uint16_t _queue_limit;

  float _margin;

  uint16_t _deny_count      = 0;
  uint16_t _accept_count    = 0;
  uint16_t _serviced_count  = 0;
  uint16_t _service_limit   = 0;

  const client_t& get_service() const;
public: 
  BaseSimulation(const uint16_t& queue_limit = 0, const uint16_t& service_limit = 0,
                 const float& margin = 0.0f);

  void simulate(float simulate_time,
                const std::function<void()>& setup_function,
                const std::function<void()>& after_service);



  void add_task(const tk_t& task);
  void pop_task();
  const tk_t& get_task() const;

  void add_client(const cl_t& client);
  void pop_client();
  cl_t& get_client();



  inline const uint16_t& get_limit() {return _queue_limit;}
  void status() const; 

  inline metric_t get_metric() const {
    return {_accept_count, _serviced_count, _deny_count};
  }
};

typedef BaseSimulation<client_t, task_t> sim_t;

extern template class BaseSimulation<client_t, task_t>;

#endif // !SIMULATION_H
