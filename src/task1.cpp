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
  Cycles,
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
              << "Seed\nFileOutput\nSimTime\nCycles\n"
              << "QueueLimit\nServiceLimit\nMargin\n"
              << "ServiceTime\nClientFlow\nStrictTimings\n";
    return;
  }
  std::ofstream output(argv[FileOutput]);

  std::srand(std::atoi(argv[Seed]));

  uint16_t sim_time = std::atoi(argv[SimTime]);
  uint16_t queue_limit = std::atoi(argv[QueueLimit]);
  uint16_t service_limit = std::atoi(argv[ServiceLimit]);
  uint16_t cycles = std::atoi(argv[Cycles]);

  #ifdef DEBUG
  std::cout << "q_lim:" << queue_limit << " s_lim:" << service_limit << "\n";
  #endif

  float margin = std::atof(argv[Margin]),
        service_time = std::atof(argv[ServiceTime]),
        client_flow = std::atof(argv[ClientFlow]);
  bool strict_time = !std::strcmp(argv[StrictTimings], "true") ? true : false;

  output << sim_time << " " << queue_limit << " " << service_limit << " "
         << service_time << " " << client_flow << " ";

  Distribution::pois_dt client_disrt(margin, client_flow);
  Distribution::pois_dt service_distr(margin, service_time);

  sim_t::metric_t* median = new sim_t::metric_t[cycles];

  for(uint16_t i=0; i<cycles; ++i) {
    #ifdef DEBUG
    std::cout << "Cycle n:" << i << "\n";
    #endif

    sim_t simulation(queue_limit, service_limit, margin);

    float time = 0.0f;
    std::function<void()> repeat_status;
    repeat_status = [&simulation, &repeat_status, &time, &output](){
      sim_t::metric_t metric = simulation.get_metric();
      #ifdef DETAILED
      output << time << " " << metric.accept << " "
             << metric.serviced << " " << metric.deny << "\n";
      #endif
      #ifdef DEBUG
      std::cout << "\nTime: " << time << "\n";
      simulation.status();
      #endif
      time += status_step;
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
    
    median[i] = simulation.get_metric();
    #ifndef DETAILED
    output << median[i].accept << " " << median[i].deny << " " << median[i].serviced << "\n";
    #endif
  }

  struct {
    float accept = 0;
    float deny = 0;
    float serviced = 0;
  } res_metric;

  for(uint16_t i=0; i<cycles; ++i) {
    res_metric.accept += (float)median[i].accept / cycles;
    res_metric.deny += (float)median[i].deny / cycles;
    res_metric.serviced += (float)median[i].serviced / cycles;
  }
  output.close();
  delete [] median;

  std::cout << "Results:\n\tAccepted: " << res_metric.accept
            << "\n\tDeny: " << res_metric.deny
            << "\n\tServiced: " << res_metric.serviced << "\n";
}
