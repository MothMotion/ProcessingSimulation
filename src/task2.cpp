#include "main.h"
#include "simulation.h"
#include "pois_distr.h"
#include "exp_distr.h"

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
constexpr float service_time_step = 1.0f;
constexpr char out_separator = ';';
constexpr char out_new_line = '\n';

void task2(int argc, char *argv[]) {
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

  // Main simulation body
    #ifdef OUTPUT_HEAD
    output << sim_time << out_separator << queue_limit << out_separator << service_limit << out_separator
          << service_time << out_separator << client_flow << out_new_line;
    #endif 

    Distribution::pois_dt client_disrt(margin, client_flow);
    Distribution::exp_dt service_distr(margin, 1/service_time);
  

    metric_t* median = new metric_t[cycles];

    // Simulation setup
    for(uint16_t i=0; i<cycles; ++i) {
      #ifdef DEBUG
      std::cout << "Cycle n:" << i << "\n";
      #endif

      sim_t simulation(queue_limit, service_limit, margin);

      // Simulation status output every 1 time unit
      float time = 0.0f;
      std::function<void()> repeat_status;
      repeat_status = [&simulation, &repeat_status, &time, &output](){
        metric_t metric = simulation.get_metric();
        #ifdef DETAILED
        output << time << out_separator << metric.accept << out_separator
              << metric.serviced << out_separator << metric.deny << out_new_line;
        #endif
        #ifdef DEBUG
        std::cout << "\nTime: " << time << "\n";
        simulation.status();
        #endif
        time += status_step;
        simulation.add_task({"status_func", 1.0f, repeat_status});
      };
      simulation.add_task({"init_status", 0.0f, repeat_status});

      // Simulate client flow
      std::function<void()> client_flow_func;
      client_flow_func = [&simulation, &strict_time, &client_disrt, &service_distr, &service_time, &client_flow, &client_flow_func](){
        #ifdef DEBUG
        std::cout << "\nAdding client\n";
        #endif
        simulation.add_client({strict_time ? service_time : service_distr.get_random()});
        simulation.add_task({"client_flow", strict_time ? client_flow : client_disrt.get_random(), client_flow_func});
      };
      client_flow_func();

      // Start simulation
      simulation.simulate(sim_time, [](){}, [](){});

      // Write results
      median[i] = simulation.get_metric();
      #ifdef OUTPUT_EACH_SIM
      output << median[i].accept << out_separator << median[i].deny << out_separator
             << median[i].serviced << out_new_line;
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

    #ifdef OUTPUT_RESULTS
    output << res_metric.accept << out_separator << res_metric.deny << out_separator
           << res_metric.serviced << out_new_line;
    #endif

    std::cout << "Results:\n\tAccepted: " << res_metric.accept
              << "\n\tDeny: " << res_metric.deny
              << "\n\tServiced: " << res_metric.serviced
              << "\n\tDeny Probability: " << res_metric.deny / (res_metric.accept + res_metric.deny) << "\n";
  
  output.close(); 
  }
