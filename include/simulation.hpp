#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "simulation.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#ifdef DEBUG
#include <cstdio>
#include <iostream>
#endif

#define tmpl template<typename cl_t, typename tk_t>
#define sim_tmpl BaseSimulation<cl_t, tk_t>

tmpl
sim_tmpl::BaseSimulation(const uint16_t& queue_lim, const uint16_t& service_lim,
                         const float& margin) {
  _queue_limit = queue_lim;
  _margin = margin;
  _service_limit = service_lim; 
}

tmpl
void sim_tmpl::simulate(float simulate_time,
                        const std::function<void()>& setup_func,
                        const std::function<void()>& after_service) {
  #ifdef DEBUG
  std::cout << "Starting simulation.\n";
  #endif
  setup_func();
  while(simulate_time > 0 && (!_client_queue.empty() || !_task_queue.empty())) {
    float delta_time = std::min(_task_queue.empty() ? MAXFLOAT : get_task().timer,
                                _service_queue.empty() ? MAXFLOAT : get_service().service_dur);
    if(delta_time == MAXFLOAT || simulate_time < delta_time)
      return;
    simulate_time -= delta_time;

    #ifdef DEBUG
    std::cout << "Delta: " << delta_time << "\n";
    std::cout << "Tasks:" << _task_queue.empty() << " size:" << _task_queue.size() << "\n\t"; 
    #endif

    uint16_t process_task_size = _task_queue.size();
    for(uint16_t i = 0; i < process_task_size;) {
      task_t& task = _task_queue[i];
      #ifdef DEBUG
      std::cout << "|(" << task.name << ")";
      #endif
      task.timer -= delta_time;

      if(task.timer < _margin) {
        #ifdef DEBUG
        std::cout << "X(" << task.name << ")";
        #endif
        --process_task_size;
        if(task.func)
          task.func();
        _task_queue.erase(_task_queue.begin()+i); 
      } else
        ++i;
    }

    #ifdef DEBUG
    std::cout << "\nService\n\t" << std::flush;
    #endif

    uint16_t process_service_size = _service_queue.size();
    for(uint16_t i = 0; i < process_service_size;) {
      client_t& cl = _service_queue[i];
      #ifdef DEBUG
      std::cout << cl.service_dur << " ";
      #endif
      cl.service_dur -= delta_time;

      if(cl.service_dur < _margin) {
        --process_service_size;
        after_service();
        ++_serviced_count;

        _service_queue.erase(_service_queue.begin()+i);

        if(!_client_queue.empty()) {
          _service_queue.push_back(get_client());
          pop_client();
        }
      } else
        ++i;
    }
  }
}

tmpl
void sim_tmpl::add_task(const tk_t& task) {
  #ifdef DEBUG
  std::cout << "Adding task " << task.name << ".\n";
  #endif
  _task_queue.push_back(task);
  #ifdef DEBUG
  std::cout << "Success\n";
  #endif
}

tmpl
void sim_tmpl::pop_task() {
  #ifdef DEBUG
  std::cout << "Removing task.\n";
  #endif
  return _task_queue.pop_back();
}

tmpl
const tk_t& sim_tmpl::get_task() const {
  #ifdef DEBUG
  std::cout << "Getting min task\n";
  #endif
  return *std::min_element(_task_queue.begin(), _task_queue.end());
}

tmpl
void sim_tmpl::add_client(const cl_t& cl) {
  if(_service_queue.size() < _service_limit) {
    #ifdef DEBUG
    std::cout << "Added to exectution client, time: " << cl.service_dur << "\n";
    #endif
    ++_accept_count;
    return _service_queue.push_back(cl);
  }

  #ifdef DEBUG
  std::cout << "Added to queue client.\n";
  #endif
  if(_client_queue.size() < _queue_limit) {
    #ifdef DEBUG
    std::cout << "Added to queue client.\n";
    #endif
    ++_accept_count;
    return _client_queue.push_back(cl);
  }

  ++_deny_count;
}

tmpl
void sim_tmpl::pop_client() {
  #ifdef DEBUG
  std::cout << "Deleted from queue client.\n";
  #endif
  _client_queue.pop_front();
}

tmpl
cl_t& sim_tmpl::get_client() {
  return _client_queue.front();
}

tmpl
void sim_tmpl::status() const {
  #ifdef DEBUG
  std::cout << "Sim: ser_s:" << _service_queue.empty() << " cl_s:" << _client_queue.empty() << std::endl;
  std::cout << "Simulation status:\n\tIn service:\n\t";
  for(uint16_t i=0; i<_service_limit; ++i)
    std::cout << (i<_service_queue.size() ? "|" : " ");
  std::cout << "\n\tIn queue:\n\t";
  for(uint16_t i=0; i<_queue_limit; ++i)
    std::cout << (i<_client_queue.size() ? "|" : " ");

  metric_t metric = get_metric();
  std::cout << "\nA:" << metric.accept
            << " D:" << metric.deny
            << " S:" << metric.serviced << "\n" << std::flush;
  #endif
}

tmpl
const client_t& sim_tmpl::get_service() const {
  return *std::min_element(_service_queue.begin(), _service_queue.end());
}

#undef tmpl
#undef sim_tmpl

template class BaseSimulation<client_t, task_t>;

#endif
