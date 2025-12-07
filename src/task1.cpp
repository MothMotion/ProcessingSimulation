#include "main.h"
#include "simulation.h"
#include "pois_distr.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

typedef enum InputEnum {
  Seed = 1,
  FileOutput,
  SimTime,
  QueueLimit,
  ServiceLimit,
  Margin,
  ServiceTime,
  ClientFlow,
  StrictTimings,
  InpSize
} inp_e;

constexpr float status_step = 1.0f;

void task1(int argc, char *argv[]) {
  if(argc < InpSize) {
    std::cout << "Failed: invalid amount arguments\n"
              << "Seed\nFileOutput\nSimTime\nQueueLimit\n"
              << "ServiceLimit\nMargin\nServiceTime\n"
              << "ClientFlow\nStrictTimings\n";
    return;
  }
  std::ofstream output(argv[FileOutput]);

  std::srand(std::atoi(argv[Seed]));

  uint16_t sim_time = std::atoi(argv[SimTime]);
  uint16_t queue_limit = std::atoi(argv[QueueLimit]);
  uint16_t service_limit = std::atoi(argv[ServiceLimit]);

  std::cout << "q_lim:" << queue_limit << " s_lim:" << service_limit << "\n";

  float margin = std::atof(argv[Margin]),
        service_time = std::atof(argv[ServiceTime]),
        client_flow = std::atof(argv[ClientFlow]);
  bool strict_time = !std::strcmp(argv[StrictTimings], "true") ? true : false;

  Distribution::pois_dt client_disrt(margin, client_flow);
  Distribution::pois_dt service_distr(margin, service_time);

  sim_t simulation(queue_limit, service_limit, margin);

  float time = 0.0f;
  std::function<void()> repeat_status;
  repeat_status = [&simulation, &repeat_status, &time, &output](){
    sim_t::metric_t metric = simulation.get_metric();
    output << time << " " << metric.accept << " "
           << metric.serviced << " " << metric.deny << "\n";
    std::cout << "\nTime: " << time << "\n";
    time += status_step;
    simulation.status(); 
    simulation.add_task({"status_func", 1.0f, repeat_status});
  };
  simulation.add_task({"init_status", 0.0f, repeat_status});

  std::function<void()> client_flow_func;
  client_flow_func = [&simulation, &strict_time, &client_disrt, &service_distr, &service_time, &client_flow, &client_flow_func](){
    #ifdef DEBUG
    std::cout << "\nAdding client\n";
    #endif
    simulation.add_client({strict_time ? service_time : service_distr.get_random()});
    simulation.add_task({"client_flow", strict_time ? client_flow : client_disrt.get_random(), client_flow_func});
  };
  client_flow_func(); 

  simulation.simulate(sim_time, [](){}, [](){});

  output.close();

  sim_t::metric_t metric = simulation.get_metric();

  std::cout << "Results:\n\tAccepted: " << metric.accept
            << "\n\tDeny: " << metric.deny
            << "\n\tServiced: " << metric.serviced << "\n";
}
