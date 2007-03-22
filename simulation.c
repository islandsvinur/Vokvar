
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <rfftw.h>

#include "simulation.h"
#include "vector.h"

Simulation *
new_simulation(int32_t dimension) {
  Simulation *s;
  size_t dim;

  s = malloc(sizeof(Simulation));
  assert(s != NULL);

  s->dimension = dimension;

  dim         = dimension * 2 * (dimension/2 + 1) * sizeof(fftw_real);
  s->u        = (fftw_real*) malloc(dim);
  s->v        = (fftw_real*) malloc(dim);
  s->u0       = (fftw_real*) malloc(dim);
  s->v0       = (fftw_real*) malloc(dim);

  dim         = dimension * dimension * sizeof(fftw_real);
  s->u_u0     = (fftw_real*) malloc(dim);
  s->u_v0     = (fftw_real*) malloc(dim);
  s->rho      = (fftw_real*) malloc(dim);
  s->rho0     = (fftw_real*) malloc(dim);

  s->plan_rc  = rfftw2d_create_plan(dimension, dimension, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
  s->plan_cr  = rfftw2d_create_plan(dimension, dimension, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

  return s;
}

void
simulation_destroy(Simulation *s) {
  free(s->u);     free(s->v);
  free(s->u0);    free(s->v0);
  free(s->u_u0);  free(s->u_v0);
  free(s->rho);   free(s->rho0);
  rfftwnd_destroy_plan(s->plan_rc);
  rfftwnd_destroy_plan(s->plan_cr);
  free(s);
}

void
simulation_set_forces(Simulation *s) {
  int i;
  for (i = 0; i < s->dimension * s->dimension; i++)
  {
    s->rho0[i] = 0.995 * s->rho[i];
    s->u_u0[i] *= 0.85; s->u_v0[i] *= 0.85;
    s->u0[i] = s->u_u0[i]; s->v0[i] = s->u_v0[i];
  }
}

void
simulation_stable_solve(Simulation *s, fftw_real viscosity, fftw_real dt) {
  fftw_real x, y, x0, y0, f, r, U[2], V[2], p, q;
  int i, j, i0, j0, i1, j1;
  int32_t n = s->dimension;

  for (i=0;i<n*n;i++)
  { s->u[i] += dt*s->u0[i]; s->u0[i] = s->u[i]; s->v[i] += dt*s->v0[i]; s->v0[i] = s->v[i]; }

  for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
     for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
     {
        x0 = n*(x-dt*s->u0[i+n*j])-0.5f;
        y0 = n*(y-dt*s->v0[i+n*j])-0.5f;
        i0 = floor(x0); p = x0-i0;
        i0 = (n+(i0%n))%n;
        i1 = (i0+1)%n;
        j0 = floor(y0); q = y0-j0;
        j0 = (n+(j0%n))%n;
        j1 = (j0+1)%n;
        s->u[i+n*j] = (1-p)*((1-q)*s->u0[i0+n*j0]+q*s->u0[i0+n*j1])+ 
        p *((1-q)*s->u0[i1+n*j0]+q*s->u0[i1+n*j1]);
        s->v[i+n*j] = (1-p)*((1-q)*s->v0[i0+n*j0]+q*s->v0[i0+n*j1])+
        p *((1-q)*s->v0[i1+n*j0]+q*s->v0[i1+n*j1]);
     }

  for(i=0; i<n; i++)
    for(j=0; j<n; j++)
    {  s->u0[i+(n+2)*j] = s->u[i+n*j]; s->v0[i+(n+2)*j] = s->v[i+n*j]; }

  rfftwnd_one_real_to_complex(s->plan_rc,
      (fftw_real *)s->u0,
      (fftw_complex*)s->u0);
  rfftwnd_one_real_to_complex(s->plan_rc,
      (fftw_real *)s->v0,
      (fftw_complex*)s->v0);

  for (i=0;i<=n;i+=2)
  {
     x = 0.5f*i;
     for (j=0;j<n;j++)
     {
        y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
        r = x*x+y*y;
        if ( r==0.0f ) continue;
        f = (fftw_real)exp(-r*dt*viscosity);
        U[0] = s->u0[i  +(n+2)*j]; V[0] = s->v0[i  +(n+2)*j];
        U[1] = s->u0[i+1+(n+2)*j]; V[1] = s->v0[i+1+(n+2)*j];

        s->u0[i  +(n+2)*j] = f*( (1-x*x/r)*U[0]     -x*y/r *V[0] );
        s->u0[i+1+(n+2)*j] = f*( (1-x*x/r)*U[1]     -x*y/r *V[1] );
        s->v0[i+  (n+2)*j] = f*(   -y*x/r *U[0] + (1-y*y/r)*V[0] );
        s->v0[i+1+(n+2)*j] = f*(   -y*x/r *U[1] + (1-y*y/r)*V[1] );
     }
  }

  rfftwnd_one_complex_to_real(s->plan_cr,
      (fftw_complex *)s->u0,
      (fftw_real*)s->u0);
  rfftwnd_one_complex_to_real(s->plan_cr,
      (fftw_complex *)s->v0,
      (fftw_real*)s->v0);

  f = 1.0/(n*n);
  for (i=0;i<n;i++)
     for (j=0;j<n;j++)
     { s->u[i+n*j] = f*s->u0[i+(n+2)*j]; s->v[i+n*j] = f*s->v0[i+(n+2)*j]; }
}

void
simulation_diffuse_matter(Simulation *s, fftw_real dt) {
  fftw_real x, y, x0, y0, p, q;
  int i, j, i0, j0, i1, j1;
  int32_t n = s->dimension;

  for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n ) {
    for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n ) {
      x0 = n*(x-dt*s->u[i+n*j])-0.5f;
      y0 = n*(y-dt*s->v[i+n*j])-0.5f;
      i0 = floor(x0);
      p = x0 - i0;
      i0 = (n + (i0 % n)) % n;
      i1 = (i0 + 1) % n;
      j0 = floor(y0);
      q = y0 - j0;
      j0 = (n + (j0 % n)) % n;
      j1 = (j0 + 1) % n;
      s->rho[i+n*j] = 
         (1 - p) * ((1 - q) * s->rho0[i0+n*j0] + q * s->rho0[i0+n*j1]) + 
               p * ((1 - q) * s->rho0[i1+n*j0] + q * s->rho0[i1+n*j1]);
    }
  }
}

Vector *
simulation_interpolate_speed(Simulation *s, Vector *v) {
  int x = floor(v->x); int y = floor(v->y);

  float ratio;
  float result_u, result_v;

  /* Linear interpolation */
  ratio = v->x - x;
  result_u = ratio * s->u[y * s->dimension + x] + (1 - ratio) * s->u[y * s->dimension + x + 1];
  ratio = v->y - y;
  result_v = ratio * s->v[y * s->dimension + x] + (1 - ratio) * s->v[y * s->dimension + x + 1];
  
  return new_vector(result_u, result_v);
}

float
simulation_value(Simulation *s, int x, int y) {
  return s->rho[y * s->dimension + x];
}

float
simulation_value_interpolated(Simulation *s, float x, float y) {
  float p1, p2, p3, p4;
  int xl = floor(x);
  int yl = floor(y);

  p1 = s->rho[xl * s->dimension + xl];
  p2 = s->rho[yl * s->dimension + xl + 1];
  p3 = s->rho[(yl+1) * s->dimension + xl];
  p4 = s->rho[(yl+1) * s->dimension + xl + 1];

  // return s->rho[y * s->dimension + x];
  return 0.2;
}

float
simulation_maximal_value(Simulation *s) {
  int i;
  float max = 0.0;
  for (i = 0; i < s->dimension * s->dimension; i++) {
     max = (((max) > (s->rho[i])) ? (max) : (s->rho[i]));
  }
  return max;
}

Simulation_statistics *
simulation_statistics(Simulation *s) {
  Simulation_statistics *st;
  st = (Simulation_statistics*) malloc(sizeof(Simulation_statistics));

  int i;
  int dim = pow(s->dimension, 2.0);
  float max = -HUGE_VAL;
  float min = HUGE_VAL;
  float mean = 0.0;

  for (i = 0; i < dim; i++) {
     max = (((max) > (s->rho[i])) ? (max) : (s->rho[i]));
     min = (((min) < (s->rho[i])) ? (min) : (s->rho[i]));
     mean += s->rho[i];
  }

  st->max = max;
  st->min = min;
  st->mean = mean / pow(s->dimension, 2.0);

  return st;
}

