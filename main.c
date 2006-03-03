
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "simulation.h"

#define DIM 50

State *state;

int
main(int argc, char **argv) {
  state = malloc(sizeof(State));
  state->dt = 0.4;
  state->color_dir = 0;
  state->vec_scale = 1000;
  state->visc = 0.001;
  state->draw_smoke = 0;
  state->draw_vecs = 1;
  state->scalar_col = 0;
  state->frozen = 0;

  state->simulation = new_simulation(DIM);
  state->visualization = new_visualization(argc, argv, 
      state->simulation, 500, 500);

  visualization_start(state->visualization);

  return 0;
}

