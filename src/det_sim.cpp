#include "det_sim.h"

#include <cmath>

void dsim_t::simulate(float simulate_time,
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

    for(dclient_t& cl : _client_queue)
      cl.in_queue += delta_time;

    #ifdef DEBUG
    std::cout << "\nService\n\t" << std::flush;
    #endif

    uint16_t process_service_size = _service_queue.size();
    for(uint16_t i = 0; i < process_service_size;) {
      dclient_t& cl = _service_queue[i];
      #ifdef DEBUG
      std::cout << cl.service_dur << " ";
      #endif
      cl.service_dur -= delta_time;
      cl.in_service += delta_time;

      if(cl.service_dur < _margin) {
        --process_service_size;
        after_service();
        _dmetric.t_in_service += cl.in_service; 
        ++_serviced_count;

        _service_queue.erase(_service_queue.begin()+i);

        if(!_client_queue.empty()) {
          _service_queue.push_back(get_client());
          _dmetric.t_in_queue += get_client().in_queue;
          pop_client();
        }
      } else
        ++i;
    }
  }
}

dmetric_t& dsim_t::get_d_metric() {
  return _dmetric;
}


template class BaseSimulation<dclient_t, task_t>;

