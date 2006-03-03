
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <assert.h>
#include <math.h>

#include "main.h"
#include "visualization.h"
#include "simulation.h"

Visualization *v;

void _set_color(float x, float y, int col);
void _cpalet(float value, float *R, float *G, float *B);
void _set_cpalet(Visualization *v, float value);

void _display(void);
void _reshape(int width, int height);
void _idle(void);
void _keyboard(unsigned char key, int x, int y);
void _drag(int mx, int my);

Visualization *
new_visualization(int argc, char **argv, Simulation *s, int width, int height) {
  v = malloc(sizeof(Visualization));
  assert(v != NULL);

  v->width = width;
  v->height = height;
  v->frozen = 0;
  v->viscosity = 0.001;
  v->timestep = 0.4;
  v->simulation = s;
  v->color_dir = 0;
  v->vector_scale = 1000;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("Real-time wispy smoke");
  glutDisplayFunc(_display);
  glutReshapeFunc(_reshape);
  glutIdleFunc(_idle);
  glutKeyboardFunc(_keyboard);
  glutMotionFunc(_drag);


  return v;
}

void
visualization_destroy(Visualization *v) {
  free(v);
}

void 
visualization_start(Visualization *v) {
  glutMainLoop();
}

void 
visualization_stop(Visualization *v) {
}

void
visualization_draw_field(Visualization *v) {
  Simulation *s = v->simulation;
  int        i, j, idx; double px,py;
  fftw_real  wn = (fftw_real)v->width / (fftw_real)(s->dimension + 1);   /* Grid element width */
  fftw_real  hn = (fftw_real)v->height / (fftw_real)(s->dimension + 1);  /* Grid element height */

  if (v->draw_smoke)
  {
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (j = 0; j < s->dimension - 1; j++)     //draw smoke
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
      _set_cpalet(v, s->rho[idx]);
      glVertex2f(px, py);
      px = wn + (fftw_real)(i + 1) * wn;
      py = hn + (fftw_real)j * hn;
      idx = (j * s->dimension) + (i + 1);
      _set_cpalet(v, s->rho[idx]);
      glVertex2f(px, py);
    }

    px = wn + (fftw_real)(s->dimension - 1) * wn;
    py = hn + (fftw_real)(j + 1) * hn;
    idx = ((j + 1) * s->dimension) + (s->dimension - 1);
    _set_cpalet(v, s->rho[idx]);
    glVertex2f(px, py);
    glEnd();
  }
  }

  if (v->draw_vectors)
  {
  glBegin(GL_LINES);        //draw velocities
  for (i = 0; i < s->dimension; i++)
    for (j = 0; j < s->dimension; j++)
    {
    idx = (j * s->dimension) + i;
    _set_color(s->u[idx], s->v[idx],v->color_dir);
    glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
    glVertex2f((wn + (fftw_real)i * wn) + v->vector_scale * s->u[idx], (hn + (fftw_real)j * hn) + v->vector_scale * s->v[idx]);
    }
  glEnd();
  }

}


/******************************************************************************
                               Private functions
******************************************************************************/


void
_cpalet(float value, float *R, float *G, float *B) {
   const float dx=0.8;

   if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;
   *R = max(0.0,(3-fabs(value-4)-fabs(value-5))/2);
   *G = max(0.0,(4-fabs(value-2)-fabs(value-4))/2);
   *B = max(0.0,(3-fabs(value-1)-fabs(value-2))/2);
}

void
_set_cpalet(Visualization *v, float value) {
   float R,G,B; int NLEVELS = 7;
   switch(v->scalar_coloring)
   {
   default:
   case 0:  R = G = B = value; break;
   case 1:  _cpalet(value,&R,&G,&B); break;
   case 2:  value *= NLEVELS; value = (int)(value); value/= NLEVELS;
      _cpalet(value,&R,&G,&B); break;
   }
   glColor3f(R,G,B);
}

void
_set_color(float x, float y, int col) {
  float r,g,b,f;
  if (col)
  {
    f = atan2(y,x) / 3.1415927 + 1;
    r = f;
    if(r > 1) r = 2 - r;
    g = f + .66667;
          if(g > 2) g -= 2;
    if(g > 1) g = 2 - g;
    b = f + 2 * .66667;
    if(b > 2) b -= 2;
    if(b > 1) b = 2 - b;
  }
  else
  { r = g = b = 1; }
  glColor3f(r,g,b);
}


/******************************************************************************
                               Callback functions
******************************************************************************/


void 
_display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  visualization_draw_field(v);
  glFlush(); 
  glutSwapBuffers();
}

void 
_reshape(int width, int height) {
  glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
  v->width = width; 
  v->height = height;
}

void 
_idle(void) {
  if (!v->frozen)
  {
    simulation_set_forces(v->simulation);
    simulation_stable_solve(v->simulation, v->viscosity, v->timestep);
    simulation_diffuse_matter(v->simulation, v->timestep);
    glutPostRedisplay();
  }
}

void 
_keyboard(unsigned char key, int x, int y) {
  switch (key)
  {
    case 't': v->timestep -= 0.001; break;
    case 'T': v->timestep += 0.001; break;
    case 'c': v->color_dir = 1 - v->color_dir; break;
    case 'S': v->vector_scale *= 1.2; break;
    case 's': v->vector_scale *= 0.8; break;
    case 'V': v->viscosity *= 5; break;
    case 'v': v->viscosity *= 0.2; break;
    case 'x': v->draw_smoke = 1 - v->draw_smoke;
        if (v->draw_smoke==0) v->draw_vectors = 1; break;
    case 'y': v->draw_vectors = 1 - v->draw_vectors;
        if (v->draw_vectors==0) v->draw_smoke = 1; break;
    case 'm': v->scalar_coloring++; if (v->scalar_coloring==NUM_SCALAR_COL_METHODS) v->scalar_coloring=0; break;
    case 'a': v->frozen = 1-v->frozen; break;
    case 'q': exit(0);
  }
}

void 
_drag(int mx, int my) {
  int xi,yi,X,Y; double  dx, dy, len;
  static int lmx=0,lmy=0;       //remembers last mouse location

  // Compute the array index that corresponds to the cursor location
  xi = (int)floor((double)(v->simulation->dimension + 1) * ((double)mx / (double)v->width));
  yi = (int)floor((double)(v->simulation->dimension + 1) * ((double)(v->height - my) / (double)v->height));

  X = xi; Y = yi;

  if (X > (v->simulation->dimension - 1))  X = v->simulation->dimension - 1; if (Y > (v->simulation->dimension - 1))  Y = v->simulation->dimension - 1;
  if (X < 0) X = 0; if (Y < 0) Y = 0;

  // Add force at the cursor location
  my = v->height - my;
  dx = mx - lmx; dy = my - lmy;
  len = sqrt(dx * dx + dy * dy);
  if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
  v->simulation->u_u0[Y * v->simulation->dimension + X] += dx; v->simulation->u_v0[Y * v->simulation->dimension + X] += dy;
  v->simulation->rho[Y * v->simulation->dimension + X] = 10.0f;
  lmx = mx; lmy = my;
}
