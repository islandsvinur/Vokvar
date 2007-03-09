#include "isolines.h"

void _draw_isoline(Visualization *v, Vector *start);

void
isolines_draw(Visualization *v) {
  _draw_isoline(v, new_vector(25, 25));
  /*glBegin(GL_POLYGON);
  glVertex2f(100, 100);
  glVertex2f(100, v->height - 100);
  glVertex2f(v->width - 100, v->height - 100);
  glVertex2f(v->width - 100, 100);
  glEnd();*/
}

void
_draw_isoline(Visualization *v, Vector *start) {
  Simulation *s = v->simulation;
  Vector *r[8];
  int dim = s->dimension, i;
  Vector *point = start;
  float p, q;
  float ratio1, ratio2;
  float isovalue = s->rho[(int)((point->x * dim) + point->y)];

  Vector *ratio = new_vector(v->width / s->dimension, 
                             v->height / s->dimension);

  visualization_set_color_palette(v, 1);
  glBegin(GL_LINE_STRIP);
  
  r[0] = new_vector(point->x - 1, point->y - 1);
  r[1] = new_vector(point->x - 1, point->y);
  r[2] = new_vector(point->x - 1, point->y + 1);
  r[3] = new_vector(point->x,     point->y + 1);
  r[4] = new_vector(point->x,     point->y - 1);
  r[5] = new_vector(point->x + 1, point->y);
  r[6] = new_vector(point->x + 1, point->y + 1);
  r[7] = new_vector(point->x,     point->y - 1);

  /*DRAW_POINT_AND_FREE(point); */
  glVertex2f(point->x * ratio->x, point->y * ratio->y);

  for ( i=0; i<8; i++ ) {
    p = s->rho[(int)r[i]->x] * dim + s->rho[(int)r[i]->y]; 
    q = s->rho[(int)r[(i+1)%8]->x] * dim + s->rho[(int)r[(i+1)%8]->y];
    if (p >= isovalue && isovalue >= q) {
      if (i == 0 || i == 1 || i == 4 || i == 5) {
        ratio1 = (p - isovalue) / (p - q);
        ratio2 = 0;
      } else {
        ratio1 = 0;
        ratio2 = (p - isovalue) / (p - q);
      }
      point = new_vector(r[i]->x + ratio1, r[(i+1)%8]->y + ratio2);
    }
  }

  free(r[0]); free(r[1]); free(r[2]); free(r[3]); 
  free(r[4]); free(r[5]); free(r[6]); free(r[7]); 

  /* DRAW_POINT_AND_FREE(point); */
  glVertex2f(point->x * ratio->x, point->y * ratio->y);

  glEnd();
}

