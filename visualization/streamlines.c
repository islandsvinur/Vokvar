#include "../global.h"
#include "streamlines.h"

void
_drawsingle(Visualization *v, Vector *start) {
  Vector *x0 = copy_vector(start); /* Current location */
  Vector *x1; /* Next location */
  Vector *v0; /* Velocity at current location */
  Vector *v0_dt; /* Distance vector */
  Vector *point;
  int i; 
  
  int length = v->vector_scale;
  Simulation *s = v->simulation;
  Vector *ratio = v->ratio;

  glLineWidth(3);
  glBegin(GL_LINE_STRIP);
  point = vector_mul(start, ratio);
  DRAW_POINT_AND_FREE(point);

  for ( i=0; i<length; i++ ) {
    v0 = simulation_interpolate_speed(s, x0);
    /* VDEBUG("%f %f", x0->x, x0->y); */
    /* _set_color(v0->x, v0->y, v->color_dir); */
    /* glColor3f(s->rho[idx], s->rho[idx], s->rho[idx]); */
    visualization_set_color_palette(v, (1-(float)i/length));
    /* Euler's numerical integration method */
    v0_dt = vector_scal_mul(v0, 10);
    x1 = vector_add(x0, v0_dt);

    del_vector(v0);
    del_vector(v0_dt);
    del_vector(x0);
    x0 = x1;

    point = vector_mul(x1, ratio);
    DRAW_POINT_AND_FREE(point);
    /* Now check whether the point is outside the viewport to start a new line
     * piece */
    if (vector_normalize(x1, s->dimension)) {
      point = vector_mul(x1, ratio);
      glEnd(); glBegin(GL_LINE_STRIP);
      DRAW_POINT_AND_FREE(point);
    }
  }
  del_vector(x0);
  glEnd();

  glPointSize(10.0);
  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_BLEND);
  glEnable(GL_POINT_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POINTS);
  point = vector_mul(start, ratio);
  glVertex2f(point->x, point->y);
  del_vector(point);
  del_vector(start);
  glEnd();

}

void
streamlines_draw(Visualization *v) {
  _drawsingle(v, new_vector(12, 12));
  _drawsingle(v, new_vector(25, 12));
  _drawsingle(v, new_vector(37, 12));
  _drawsingle(v, new_vector(25, 25));
  _drawsingle(v, new_vector(12, 37));
  _drawsingle(v, new_vector(25, 37));
  _drawsingle(v, new_vector(37, 37));
}
