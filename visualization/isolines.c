#include "isolines.h"
#include "../simulation.h"
#include <rfftw.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void _draw_isoline_point(Visualization *v, Vector *start);
void _draw_isoline_value(Visualization *v, float value);
void Contour(Visualization *v, int nc, double *z);

void
isolines_draw(Visualization *v) {
  static int n = 10;
  double *levels;
  malloc(sizeof(levels) * n);
  Simulation *s = v->simulation;
  int dim = s->dimension, i;
  _draw_isoline_value(v, 0.5);
}

void
_draw_isoline_point(Visualization *v, Vector *start) {
  Simulation *s = v->simulation;
  int dim = s->dimension, i;
  _draw_isoline_value(v, simulation_value(s, start->x, start->y));
}

void
_draw_isoline_value(Visualization *v, float value) {
  Simulation *s = v->simulation;
#define VAL(x,y) simulation_value(s, x, y)

  float ratio1, ratio2;
  int dim = s->dimension, i;
  Vector *ratio = new_vector(v->width / dim, v->height / dim);

  float min, max;
  float x, y;
  for (x = 0; x < dim - 1; x++) {
    for (y = 0; y < dim - 1; y++) {
      min = MIN( MIN(VAL(x, y),     VAL(x, y + 1)), 
                 MIN(VAL(x + 1, y), VAL(x + 1, y + 1))
               );
      max = MAX( MAX(VAL(x, y),     VAL(x, y + 1)), 
                 MAX(VAL(x + 1, y), VAL(x + 1, y + 1))
               );

      // Triangle lies entirely above or under the value
      if (max < value || min > value) continue;
      glBegin(GL_QUADS);
      glColor3f(1.0, 1.0, 1.0);
      glVertex2f(x * ratio->x, y * ratio->y);
      glVertex2f(x * ratio->x, (y+1) * ratio->y);
      glVertex2f((x+1) * ratio->x, (y+1) * ratio->y);
      glVertex2f((x+1) * ratio->x, y * ratio->y);
      glEnd();
    }
  }

  // visualization_set_color_palette(v, 1);
  glBegin(GL_LINE_STRIP);

  

  glEnd();
}

