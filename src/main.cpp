#include "main.h"

int main (int argc, char *argv[]) {
  #ifdef DEBUG
  debug();
  #endif
  #ifdef TASK1
  task1(argc, argv);
  #endif
  #ifdef TASK2
  task2(argc, argv);
  #endif
  #ifdef TASK3
  task3(argc, argv);
  #endif
  #ifdef TASK4
  task4(argc, argv);
  #endif
  #ifdef TASK5
  task5(argc, argv);
  #endif

  return 0;
}
