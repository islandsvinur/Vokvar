
#include <stdio.h>
#include <stdlib.h>

#include "simulation.h"

#define DIM 50


int
main(int argc, char **argv) {
  Simulation *sim;

  sim = new_simulation(DIM);

  return 0;
}

