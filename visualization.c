
#include <stdio.h>
#include <stdlib.h>

#include <glut.h>
#include <assert.h>
#include <math.h>

#include "global.h"

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

void _draw_vectors(Visualization *v);
void _draw_smoke(Visualization *v);
void _draw_streamlines(Visualization *v);
void _draw_isolines(Visualization *v);

void _draw_streamline(Visualization *v, Vector *start);
void _draw_isoline(Visualization *v, Vector *start);

Visualization *
new_visualization(int argc, char **argv, Simulation *s, int width, int height) {
  v = malloc(sizeof(Visualization));
  assert(v != NULL);

  v->width = width;
  v->height = height;

  v->frozen = 0;
  v->viscosity = 0.001;
  v->timestep = 0.2;

  v->color_dir = 0;
  v->vector_scale = 1000;

  v->draw = VIZ_SMOKE | VIZ_STREAMLINES;

  v->scalar_coloring = 0;

  v->simulation = s;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  v->main_window = glutCreateWindow("Real-time wispy smoke");

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
  v->frozen = 1;
  glutDestroyWindow(v->main_window);
}

void
visualization_draw_field(Visualization *v) {
  if (v->draw & VIZ_SMOKE)        _draw_smoke(v);
  if (v->draw & VIZ_VECTORS)      _draw_vectors(v);
  if (v->draw & VIZ_STREAMLINES)  _draw_streamlines(v);
  if (v->draw & VIZ_ISOLINES)     _draw_isolines(v);
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
    /* Timestep */
    case 't': v->timestep -= 0.001; 
              printf("Time step: %f\n", v->timestep); break;
    case 'T': v->timestep += 0.001; 
              printf("Time step: %f\n", v->timestep); break;

    /* Whether to colorize the vectors according to their direction */
    case 'c': v->color_dir = 1 - v->color_dir; break;

    /* Scale of vectors and streamlines */
    case 'S': v->vector_scale *= 1.2; 
              printf("Vector scale: %f\n", v->vector_scale); break;
    case 's': v->vector_scale *= 0.8; 
              printf("Vector scale: %f\n", v->vector_scale); break;

    /* Viscosity of the fluid */
    case 'V': v->viscosity *= 5; 
              printf("Viscosity: %f\n", v->viscosity); break;
    case 'v': v->viscosity *= 0.2; 
              printf("Viscosity: %f\n", v->viscosity); break;

    /* Visualizations to draw */
    case '1': if (v->draw & VIZ_SMOKE)        v->draw -= VIZ_SMOKE; 
                else v->draw += VIZ_SMOKE;        
              break;
    case '2': if (v->draw & VIZ_VECTORS)      v->draw -= VIZ_VECTORS; 
                else v->draw += VIZ_VECTORS;      
              break;
    case '3': if (v->draw & VIZ_STREAMLINES)  v->draw -= VIZ_STREAMLINES; 
                else v->draw += VIZ_STREAMLINES;  
              break;
    case '4': if (v->draw & VIZ_ISOLINES)     v->draw -= VIZ_ISOLINES; 
                else v->draw += VIZ_ISOLINES;     
              break;

    /* Change palette */
    case 'p': v->scalar_coloring++; 
              if (v->scalar_coloring==NUM_SCALAR_COL_METHODS) 
                v->scalar_coloring=0; 
              break;

    /* Freeze animation (and simulation) */
    case 'a': v->frozen = 1-v->frozen; 
              printf("Frozen: %i\n", v->frozen); break;

    /* Quit */
    case 'q': main_stop();
  }
}

void 
_drag(int mx, int my) {
  int xi,yi,X,Y; double  dx, dy, len;
  static int lmx=0,lmy=0;       //remembers last mouse location
  /* VDEBUG("%i %i", mx, my); */

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


/******************************************************************************
                                 Draw functions
******************************************************************************/


void
_draw_smoke(Visualization *v) {
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

void
_draw_vectors(Visualization *v) {
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

#define DRAW_POINT_AND_FREE(point) do { glVertex2f(point->x, point->y); \
                                        del_vector(point); } while (0)

void
_draw_streamlines(Visualization *v) {
  _draw_streamline(v, new_vector(12, 12));
  _draw_streamline(v, new_vector(25, 12));
  _draw_streamline(v, new_vector(37, 12));
  _draw_streamline(v, new_vector(25, 25));
  _draw_streamline(v, new_vector(12, 37));
  _draw_streamline(v, new_vector(25, 37));
  _draw_streamline(v, new_vector(37, 37));
}

void
_draw_isolines(Visualization *v) {
  _draw_isoline(v, new_vector(25, 25));
  /*glBegin(GL_POLYGON);
  glVertex2f(100, 100);
  glVertex2f(100, v->height - 100);
  glVertex2f(v->width - 100, v->height - 100);
  glVertex2f(v->width - 100, 100);
  glEnd();*/
}

void
_draw_streamline(Visualization *v, Vector *start) {
  Vector *x0 = start; /* Current location */
  Vector *x1; /* Next location */
  Vector *v0; /* Velocity at current location */
  Vector *v0_dt; /* Distance vector */
  Vector *point;
  int i; 
  
  int length = v->vector_scale;
  Simulation *s = v->simulation;
  Vector *ratio = new_vector(v->width / s->dimension, 
                             v->height / s->dimension);

  glLineWidth(3);
  glBegin(GL_LINE_STRIP);
  point = vector_mul(start, ratio);
  DRAW_POINT_AND_FREE(point);

  for ( i=0; i<length; i++ ) {
    v0 = simulation_interpolate_speed(s, x0);
    /* VDEBUG("%f %f", x0->x, x0->y); */
    /* _set_color(v0->x, v0->y, v->color_dir); */
    /* glColor3f(s->rho[idx], s->rho[idx], s->rho[idx]); */
    _set_cpalet(v, (1-(float)i/length));
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
  del_vector(ratio);

  glEnd();
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

  _set_cpalet(v, 1);
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
