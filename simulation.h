#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <rfftw.h>

#include "global.h"
#include "vector.h"

#define SDEBUG(...) DEBUG("simulation", __VA_ARGS__)

/**********************************************************************
 * Public datastructures
 **********************************************************************/

typedef struct {
  int dimension; /* Size of the data set */

  fftw_real *u, *v; /* (u,v) = velocity field */
  fftw_real *u0, *v0;
  fftw_real *u_u0, *u_v0; /* User-induced forces */
  fftw_real *rho, *rho0; /* Smoke density */

  rfftwnd_plan plan_rc, plan_cr; /* FFT plans */
} Simulation;

typedef struct {
  float max; /* maximal value in the simulation data set */
  float min; /* minimal value in the simulation data set */
  float mean; /* mean value in the simulation data set */
} Simulation_statistics;

/**********************************************************************
 * Public functions
 **********************************************************************/

/* Creates a new simulation */
Simulation *new_simulation(int dimension);
/* Delete the simulation */
void simulation_destroy(Simulation *s);

/* Set the simulation forces */
void simulation_set_forces(Simulation *s);
/* Calculates new values */
void simulation_stable_solve(Simulation *s, 
    fftw_real viscosity, fftw_real dt);
/* Lets matter diffuse */
void simulation_diffuse_matter(Simulation *s, fftw_real dt);
/* Returns the interpolated speed */
Vector *simulation_interpolate_speed(Simulation *s, Vector *v);
/* Returns the interpolated density value */
float *simulation_interpolate_density(Simulation *s, Vector *v);
/* Returns the density value at specified location */
float simulation_value(Simulation *s, int x, int y);
/* Returns the maximal density value in the data set */
float simulation_maximal_value(Simulation *s);
/* Returns a statistical information about the data set */ 
Simulation_statistics *simulation_statistics(Simulation *s);

#endif /* SIMULATION_H */
