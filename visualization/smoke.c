#include "smoke.h"

void
smoke_draw(Visualization *v) {
  int i, j, idx; double px, py;
  Simulation *s = v->simulation;
  /* Grid element sizes */
  fftw_real  wn = (fftw_real)v->width / (fftw_real)(s->dimension + 1);
  fftw_real  hn = (fftw_real)v->height / (fftw_real)(s->dimension + 1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (j = 0; j < s->dimension - 1; j++)
  {
    glBegin(GL_TRIANGLE_STRIP);

    i = 0;
    px = wn + (fftw_real)i * wn;
    py = hn + (fftw_real)j * hn;
    idx = (j * s->dimension) + i;
    glColor3f(s->rho[idx], s->rho[idx], s->rho[idx]);
    glVertex2f(px, py);

    for (i = 0; i < s->dimension - 1; i++)
    {
      px = wn + (fftw_real)i * wn;
      py = hn + (fftw_real)(j + 1) * hn;
      idx = ((j + 1) * s->dimension) + i;
      visualization_set_color_palette(v, s->rho[idx]);
      glVertex2f(px, py);
      px = wn + (fftw_real)(i + 1) * wn;
      py = hn + (fftw_real)j * hn;
      idx = (j * s->dimension) + (i + 1);
      visualization_set_color_palette(v, s->rho[idx]);
      glVertex2f(px, py);
    }

    px = wn + (fftw_real)(s->dimension - 1) * wn;
    py = hn + (fftw_real)(j + 1) * hn;
    idx = ((j + 1) * s->dimension) + (s->dimension - 1);
    visualization_set_color_palette(v, s->rho[idx]);
    glVertex2f(px, py);
    glEnd();
  }
}

