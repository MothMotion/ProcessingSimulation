#ifndef SIMULATION_H
#define SIMULATION_H

#include <cstdint>
#include <functional>
#include <queue>
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

typedef class BaseSimulation {
private:
  float _margin;
protected:
  std::queue<client_t> _client_queue;
  std::deque<task_t> _task_queue;
  std::deque<client_t> _service_queue;
  uint16_t _queue_limit;

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



  void add_task(const task_t& task);
  void pop_task();
  const task_t& get_task() const;

  void add_client(const client_t& client);
  void pop_client();
  client_t& get_client();



  inline const uint16_t& get_limit() {return _queue_limit;}
  void status() const;

  typedef struct Metrics {
    uint16_t accept, serviced, deny;
  } metric_t;

  inline metric_t get_metric() const {
    return {_accept_count, _serviced_count, _deny_count};
  }
} sim_t;

#endif // !SIMULATION_H
