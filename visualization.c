
#include <stdio.h>
#include <stdlib.h>

#include <glut.h>
#include <assert.h>
#include <math.h>

#include "global.h"

#include "visualization.h"
#include "simulation.h"

#include "visualization/isolines.h"
#include "visualization/smoke.h"
#include "visualization/streamlines.h"
#include "visualization/vectors.h"

Visualization *v;

void _set_color(float x, float y, int col);
void _cpalet(float value, float *R, float *G, float *B);

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
  v->oldwidth = width;
  v->oldheight = height;

  v->frozen = 0;
  v->fullscreen = 0;
  v->viscosity = 0.001;
  v->timestep = 0.2;

  v->color_dir = 0;
  v->vector_scale = 1000;

  v->draw = VIZ_ISOLINES;

  v->isolines_type = VIZ_ISO_BY_NUM;
  v->isolines_number = 16;

  v->scalar_coloring = 1;

  v->simulation = s;

  v->ratio = new_vector(width / s->dimension, height / s->dimension);

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
  del_vector(v->ratio);
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
  if (v->draw & VIZ_SMOKE)        smoke_draw(v);
  if (v->draw & VIZ_VECTORS)      vectors_draw(v);
  if (v->draw & VIZ_STREAMLINES)  streamlines_draw(v);
  if (v->draw & VIZ_ISOLINES)     isolines_draw(v);
}

void
visualization_set_color_palette(Visualization *v, float value) {
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

  v->oldwidth = v->width; 
  v->oldheight = v->height;
  v->width = width; 
  v->height = height;

  Vector *p = v->ratio;
  v->ratio = new_vector(width / v->simulation->dimension, height / v->simulation->dimension);
  del_vector(p);
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
    case 'c': v->color_dir = 1 - v->color_dir; 
              printf("Colorize vectors: %s\n", (v->color_dir ? "yes" : "no")); break;

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

    /* Number of isolines in 'by number' mode */
    case 'I': v->isolines_number *= 2;
              printf("Isolines count: %d\n", v->isolines_number); break;
    case 'i': v->isolines_number *= 0.5;
              if (v->isolines_number == 0) v->isolines_number = 1.0;
              printf("Isolines count: %d\n", v->isolines_number); break;

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

    case 'o': v->isolines_type++;
              if (v->isolines_type==VIZ_ISO_COUNT)
                v->isolines_type=VIZ_ISO_BY_NUM;
              printf("Isolines type: ");
              switch (v->isolines_type) {
              default: break;
              case VIZ_ISO_BY_NUM: printf("by number (%d)", v->isolines_number); break;
              case VIZ_ISO_BY_VALUE: printf("by value (%d)", v->isolines_number); break;
              case VIZ_ISO_BY_POINT: printf("by point"); break;
              }
              printf("\n");
              break;

    /* Freeze animation (and simulation) */
    case 'a': v->frozen = 1-v->frozen; 
              printf("Frozen: %s\n", (v->frozen ? "yes" : "no")); break;

    case 'f': v->fullscreen = 1 - v->fullscreen;
              if (v->fullscreen) 
                glutFullScreen();
              else
                glutReshapeWindow(v->oldwidth, v->oldheight);
              break;

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

  if (X > (v->simulation->dimension - 1))  X = v->simulation->dimension - 1; 
  if (Y > (v->simulation->dimension - 1))  Y = v->simulation->dimension - 1;
  if (X < 0) X = 0; if (Y < 0) Y = 0;

  // Add force at the cursor location
  my = v->height - my;
  dx = mx - lmx; dy = my - lmy;
  len = sqrt(dx * dx + dy * dy);
  if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
  v->simulation->u_u0[Y * v->simulation->dimension + X] += dx;
  v->simulation->u_v0[Y * v->simulation->dimension + X] += dy;
  v->simulation->rho[Y * v->simulation->dimension + X] = 10.0f;
  lmx = mx; lmy = my;
}

