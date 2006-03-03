#ifndef MAIN_H
#define MAIN_H

#include "simulation.h"
#include "visualization.h"

#define DEBUG(title,...) do { fprintf(stderr, "%s: ", title); \
                              fprintf(stderr, __VA_ARGS__); \
                              fprintf(stderr, "\n"); \
                         } while(0)

#define MDEBUG(...) DEBUG("main", __VA_ARGS__)

typedef struct {
  int    winWidth, winHeight;
  double dt;          //simulation time step
  int    color_dir;   //use direction color-coding or not
  float  vec_scale;   //scaling of hedgehogs
  float  visc;        //fluid viscosity
  int    draw_smoke;  //draw the smoke drift
  int    draw_vecs;   //draw the vector field
  int    scalar_col;  //method for scalar coloring
  int    frozen;      //toggles on/off the animation
  Simulation  *simulation;
  Visualization *visualization;
} State;

#endif /* MAIN_H */
