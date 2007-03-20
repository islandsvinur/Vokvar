
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "simulation.h"
#include "visualization.h"

#define DIM 50

Visualization *visualization;
Simulation *simulation;

void 
main_stop(void) {
  visualization_stop(visualization);
  /* simulation_stop(simulation); */

  visualization_destroy(visualization);
  simulation_destroy(simulation);

  exit(0);
}

int
main(int argc, char **argv) {
  simulation = new_simulation(DIM);
  visualization = new_visualization(argc, argv, simulation, 600, 600);

  visualization_start(visualization);

  return 0;
}
