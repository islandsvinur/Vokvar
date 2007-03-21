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

/**********************************************************************
 * Public datastructures
 **********************************************************************/

/* Visualization type selector */
typedef enum {
  VIZ_NONE = 0,
  VIZ_SMOKE = 1,
  VIZ_VECTORS = 2,
  VIZ_STREAMLINES = 4,
  VIZ_ISOLINES = 8
} Visualization_draw;

/* Isoline type selector */
typedef enum {
  VIZ_ISO_BY_NUM = 0,
  VIZ_ISO_BY_VALUE,
  VIZ_ISO_BY_POINT,
  VIZ_ISO_COUNT
} Visualization_isolines_type;

/* Visualization datastructure */
typedef struct {
  int width, oldwidth; /* Window width */
  int height, oldheight; /* Window height */

  int frozen; /* Simulation and visualization frozen? */
  int fullscreen; /* Window fullscreen? */
  int main_window; /* GL window identifier */

  float viscosity; /* Fluid viscosity value */
  float timestep; /* Time assumed between two calculaions */

  Visualization_draw draw;
  Vector *ratio; /* Ratio between data set coordinates and GL coordinates */

  int scalar_coloring; /* Palette selection */

  int color_dir; /* Color depending on direction of vectors? */
  float vector_scale; /* Scale factor for vectors vis. */

  Visualization_isolines_type isolines_type;
  void *isolines_datapoints; /* Storage for isolines module */
  int isolines_number; /* Number of isolines to draw */

  Simulation *simulation;
} Visualization;


/**********************************************************************
 * Public functions
 **********************************************************************/

/* Creates a new visualization */
Visualization *new_visualization(int argc, char **argv, 
    Simulation *simulation, int width, int height);
/* Deletes a visualization */
void visualization_destroy(Visualization *v);

/* Starts the visualization */
void visualization_start(Visualization *v);
/* Stops the visualization */
void visualization_stop(Visualization *v);

/* Draws one frame in the visualization */
void visualization_draw_field(Visualization *v);
/* Sets the current color to the corresponding value in the palette currently
 * in use */
void visualization_set_color_palette(Visualization *v, float value);

#endif /* VISUALIZATION_H */
