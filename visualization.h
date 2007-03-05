#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#if defined(_LINUX)
#include <glut.h>
#else
#include <GLUT/glut.h>
#endif

#include "global.h"

#define VDEBUG(...) DEBUG("visualization", __VA_ARGS__)

#include "main.h"
#include "simulation.h"

#define NUM_SCALAR_COL_METHODS 3

#define max(x,y) ((x > y)?x:y)

typedef enum {
  VIZ_NONE = 0,
  VIZ_SMOKE = 1,
  VIZ_VECTORS = 2,
  VIZ_STREAMLINES = 4,
  VIZ_ISOLINES = 8
} Visualization_draw;

typedef struct {
  int width;
  int height;

  int frozen;
  float viscosity;
  float timestep;

  int color_dir;
  float vector_scale;

  Visualization_draw draw;

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
