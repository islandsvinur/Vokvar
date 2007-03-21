#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <glut.h>

#include "global.h"

#define VDEBUG(...) DEBUG("visualization", __VA_ARGS__)
#define DRAW_POINT_AND_FREE(point) do { glVertex2f(point->x, point->y); \
                                        del_vector(point); } while (0)

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

typedef enum {
  VIZ_ISO_BY_NUM = 0,
  VIZ_ISO_BY_VALUE,
  VIZ_ISO_BY_POINT,
  VIZ_ISO_COUNT
} Visualization_isolines_type;

typedef struct {
  int width, oldwidth;
  int height, oldheight;

  int frozen;
  int fullscreen;
  float viscosity;
  float timestep;

  int color_dir;
  float vector_scale;

  Vector *ratio;

  Visualization_draw draw;

  Visualization_isolines_type isolines_type;

  void *isolines_datapoints;

  int isolines_number;

  int scalar_coloring;

  int main_window;

  Simulation *simulation;
} Visualization;


Visualization *new_visualization(int argc, char **argv, 
    Simulation *simulation, int width, int height);
void visualization_destroy(Visualization *v);

void visualization_start(Visualization *v);
void visualization_stop(Visualization *v);

void visualization_draw_field(Visualization *v);
void visualization_set_color_palette(Visualization *v, float value);

#endif /* VISUALIZATION_H */
