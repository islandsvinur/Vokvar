
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <rfftw.h>

#include "simulation.h"

Simulation *
new_simulation(int32_t dimension) {
  Simulation *s;

  s = malloc(sizeof(Simulation));
  assert(s != NULL);

  s->dimension = dimension;
  /* FIXME: init_FFT */

  return s;
}

void
simulation_destroy(Simulation *s) {
  /* FIXME: clean up */
  free(s);
}

void
simulation_stable_solve(Simulation *s, fftw_real viscosity, fftw_real dt) {
  /* FIXME: stable_solve */
}

void
simulation_diffuse_matter(Simulation *s, fftw_real dt) {
  /* FIXME: diffuse_matter */
}

