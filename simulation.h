#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <rfftw.h>

#include "global.h"
#include "vector.h"

#define SDEBUG(...) DEBUG("simulation", __VA_ARGS__)

typedef struct {
  int32_t dimension;

  fftw_real *u,     *v;    /* (u,v) = velocity field */
  fftw_real *u0,    *v0;
  fftw_real *u_u0,  *u_v0; /* User-induced forces */
  fftw_real *rho,   *rho0; /* Smoke density */

  rfftwnd_plan plan_rc, plan_cr;
} Simulation;

Simulation *new_simulation(int32_t dimension);
void simulation_destroy(Simulation *s);

void simulation_set_forces(Simulation *s);
void simulation_stable_solve(Simulation *s, fftw_real viscosity, fftw_real dt);
void simulation_diffuse_matter(Simulation *s, fftw_real dt);
Vector *simulation_interpolate_speed(Simulation *s, Vector *v);
float *simulation_interpolate_density(Simulation *s, Vector *v);

#endif /* SIMULATION_H */
