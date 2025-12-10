#include "ddif_sim.h"

#include <cmath>
#include <cstdint>

#include <iostream>

void ddsim_t::simulate(float simulate_time,
                       const std::function<void()>& setup_func,
                       const std::function<void()>& after_service) {
  #ifdef DEBUG
  std::cout << "Starting simulation.\n";
  #endif
  setup_func();
  _dmetric.arr_status = new float[_service_limit];
  for(uint16_t i=0; i<_service_limit; ++i)
    _dmetric.arr_status[i] = 0.0f;
  uint32_t ticks = 0; 

  while(simulate_time > _margin && (!_client_queue.empty() || !_task_queue.empty())) {
    float delta_time = std::min(_task_queue.empty() ? MAXFLOAT : get_task().timer,
                                _service_queue.empty() ? MAXFLOAT : get_service().service_dur);
    if(delta_time == MAXFLOAT || simulate_time < delta_time) {
      simulate_time = -1;
      continue;
    }
    simulate_time -= delta_time;
    ++ticks;

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

    for(dclient_t& cl : _client_queue)
      cl.in_queue += delta_time;

    #ifdef DEBUG
    std::cout << "\nService\n\t" << std::flush;
    #endif
 
    for(uint16_t i = 0; i < _service_limit; ++i) {
      ddclient_t& cl = _service_queue[i];
      if(cl.service_dur < _margin)
        continue;
      #ifdef DEBUG
      std::cout << cl.service_dur << " ";
      #endif
      cl.service_dur -= delta_time;
      cl.in_service += delta_time;
      _dmetric.arr_status[i] += 1;

      if(cl.service_dur < _margin) { 
        after_service();
        _dmetric.t_in_service += cl.in_service; 
        ++_serviced_count;

        if(!_client_queue.empty()) {
          _service_queue[i] = get_client();
          #ifdef DEBUG
          std::cout << "Executing new client: " << _service_queue[i].arr_service_dur[i];
          #endif
          _service_queue[i].service_dur = _service_queue[i].arr_service_dur[i];
          _dmetric.t_in_queue += get_client().in_queue;
          pop_client();
        }
      }
    }
  } 
  for(uint16_t i=0; i<_service_limit; ++i)
    _dmetric.arr_status[i] /= ticks;
}

void ddsim_t::add_client(const ddclient_t& cl) {
  #ifdef DEBUG
  std::cout << "HELLO????\n\n\n";
  #endif
  for(uint16_t i=0; i<_service_limit; ++i) {
    #ifdef DEBUG
    std::cout << "Added to exectution client, time: " << cl.service_dur << "\n";
    #endif
    if(_service_queue[i].service_dur < _margin) {
      _service_queue[i] = cl;
      _service_queue[i].service_dur = cl.arr_service_dur[i];
      ++_accept_count;
      return;
    }
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

const ddclient_t ddsim_t::get_service() const {
  ddclient_t min_client = _service_queue[0];
  if(min_client.service_dur < _margin)
    min_client.service_dur = MAXFLOAT;

  for(uint16_t i=1; i<_service_limit; ++i) {
    if(_service_queue[i].service_dur != 0 &&
       min_client.service_dur < _service_queue[i].service_dur)
      min_client = _service_queue[i];
  }

  return min_client;
}

const size_t ddsim_t::get_service_size() const {
  size_t res = 0;
  for(uint16_t i=0; i<_service_limit; ++i) {
    if(_service_queue[i].service_dur >= _margin)
      ++res;
  }
  return res;
}
