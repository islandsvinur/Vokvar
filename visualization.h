#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <glut.h>

#include "main.h"
#include "simulation.h"

#define VDEBUG(...) DEBUG("visualization", __VA_ARGS__)
#define NUM_SCALAR_COL_METHODS 7

#define max(x,y) ((x > y)?x:y)

typedef struct {
  int width;
  int height;

  int frozen;
  float viscosity;
  float timestep;

  int color_dir;
  float vector_scale;

  int draw_smoke;
  int draw_vectors;

  int scalar_coloring;

  int main_window;

  Simulation *simulation;
} Visualization;


Visualization *new_visualization(int argc, char **argv, Simulation *simulation, int width, int height);
void visualization_destroy(Visualization *v);

void visualization_start(Visualization *v);
void visualization_stop(Visualization *v);

void visualization_draw_field(Visualization *v);

#endif /* VISUALIZATION_H */
