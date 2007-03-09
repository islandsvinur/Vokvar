#include "../visualization.h"

void
vectors_draw(Visualization *v) {
  int i, j, idx;
  Simulation *s = v->simulation;
  /* Grid element sizes */
  fftw_real  wn = (fftw_real)v->width / (fftw_real)(s->dimension + 1);
  fftw_real  hn = (fftw_real)v->height / (fftw_real)(s->dimension + 1);

  glLineWidth(1);
  glBegin(GL_LINES);
  for (i = 0; i < s->dimension; i++)
  for (j = 0; j < s->dimension; j++) {
    idx = (j * s->dimension) + i;
    _set_color(s->u[idx], s->v[idx],v->color_dir);
    glVertex2f(wn + (fftw_real)i * wn, 
        hn + (fftw_real)j * hn);
    glVertex2f((wn + (fftw_real)i * wn) + v->vector_scale * s->u[idx], 
        (hn + (fftw_real)j * hn) + v->vector_scale * s->v[idx]);
  }
  glEnd();
}

