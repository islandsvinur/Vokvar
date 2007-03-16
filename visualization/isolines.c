#include "isolines.h"
#include <rfftw.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void _draw_isoline(Visualization *v, Vector *start);
void Contour(Visualization *v, int nc, double *z);

void
isolines_draw(Visualization *v) {
  static int n = 10;
  double *levels;
  malloc(sizeof(levels) * n);
  levels[0] = 0.0; levels[1] = 0.01;
  levels[2] = 0.02; levels[3] = 0.03;
  levels[4] = 0.04; levels[5] = 0.05; 
  levels[6] = 0.06; levels[7] = 0.07; 
  levels[8] = 0.08; levels[9] = 0.09;
  Contour(v, n, levels);
  // _draw_isoline(v, new_vector(25, 25));
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


/* Original source: http://local.wasp.uwa.edu.au/~pbourke/papers/conrec/conrec.c */
/*
   Derivation from the fortran version of CONREC by Paul Bourke
   d               ! matrix of data to contour
   ilb,iub,jlb,jub ! index bounds of data matrix
   x               ! data matrix column coordinates
   y               ! data matrix row coordinates
   nc              ! number of contour levels
   z               ! contour levels in increasing order
*/
void Contour(Visualization *v, int nc, double *z)
{
#define xsect(p1,p2) (h[p2]*xh[p1]-h[p1]*xh[p2])/(h[p2]-h[p1])
#define ysect(p1,p2) (h[p2]*yh[p1]-h[p1]*yh[p2])/(h[p2]-h[p1])

   int m1,m2,m3,case_value;
   double dmin,dmax,x1=0,x2=0,y1=0,y2=0;
   int i,j,k,m;
   double h[5];
   int sh[5];
   double xh[5],yh[5];
   int im[4] = {0,1,1,0},jm[4]={0,0,1,1};
   int castab[3][3][3] = {
     { {0,0,8},{0,2,5},{7,6,9} },
     { {0,3,4},{1,3,1},{4,3,0} },
     { {9,6,7},{5,2,0},{8,0,0} }
   };
   double temp1,temp2;

   Simulation *s = v->simulation;
   Vector *ratio = new_vector(v->width / s->dimension,
                              v->height / s->dimension);
   int dim = s->dimension;

   fftw_real *d = s->rho;

   glBegin(GL_LINES);
   for (j=(dim-1);j>=0;j--) {
      for (i=0;i<=dim -1;i++) {
         temp1 = MIN(d[i*dim + j],d[i*dim + j+1]);
         temp2 = MIN(d[i*dim+dim + j],d[i*dim+dim + j+1]);
         dmin  = MIN(temp1,temp2);
         temp1 = MAX(d[i*dim + j],d[i*dim + j+1]);
         temp2 = MAX(d[i*dim+dim + j],d[i*dim+dim + j+1]);
         dmax  = MAX(temp1,temp2);
         if (dmax < z[0] || dmin > z[nc-1])
            continue;
         for (k=0;k<nc;k++) {
            if (z[k] < dmin || z[k] > dmax)
               continue;
            for (m=4;m>=0;m--) {
               if (m > 0) {
                  h[m]  = d[(i+im[m-1]) * dim + j+jm[m-1]]-z[k];
                  xh[m] = i+im[m-1];
                  yh[m] = j+jm[m-1];
               } else {
                  h[0]  = 0.25 * (h[1]+h[2]+h[3]+h[4]);
                  xh[0] = (i+0.5);
                  yh[0] = (j+0.5);
               }
               if (h[m] > 0.0)
                  sh[m] = 1;
               else if (h[m] < 0.0)
                  sh[m] = -1;
               else
                  sh[m] = 0;
            }

            /*
               Note: at this stage the relative heights of the corners and the
               centre are in the h array, and the corresponding coordinates are
               in the xh and yh arrays. The centre of the box is indexed by 0
               and the 4 corners by 1 to 4 as shown below.
               Each triangle is then indexed by the parameter m, and the 3
               vertices of each triangle are indexed by parameters m1,m2,and m3.
               It is assumed that the centre of the box is always vertex 2
               though this isimportant only when all 3 vertices lie exactly on
               the same contour level, in which case only the side of the box
               is drawn.
                  vertex 4 +-------------------+ vertex 3
                           | \               / |
                           |   \    m-3    /   |
                           |     \       /     |
                           |       \   /       |
                           |  m=2    X   m=2   |       the centre is vertex 0
                           |       /   \       |
                           |     /       \     |
                           |   /    m=1    \   |
                           | /               \ |
                  vertex 1 +-------------------+ vertex 2
            */
            /* Scan each triangle in the box */
            for (m=1;m<=4;m++) {
               m1 = m;
               m2 = 0;
               if (m != 4)
                  m3 = m + 1;
               else
                  m3 = 1;
               if ((case_value = castab[sh[m1]+1][sh[m2]+1][sh[m3]+1]) == 0)
                  continue;
               switch (case_value) {
               case 1: /* Line between vertices 1 and 2 */
                   x1 = xh[m1];
                   y1 = yh[m1];
                   x2 = xh[m2];
                   y2 = yh[m2];
                   break;
               case 2: /* Line between vertices 2 and 3 */
                   x1 = xh[m2];
                   y1 = yh[m2];
                   x2 = xh[m3];
                   y2 = yh[m3];
                   break;
               case 3: /* Line between vertices 3 and 1 */
                   x1 = xh[m3];
                   y1 = yh[m3];
                   x2 = xh[m1];
                   y2 = yh[m1];
                   break;
               case 4: /* Line between vertex 1 and side 2-3 */
                   x1 = xh[m1];
                   y1 = yh[m1];
                   x2 = xsect(m2,m3);
                   y2 = ysect(m2,m3);
                   break;
               case 5: /* Line between vertex 2 and side 3-1 */
                   x1 = xh[m2];
                   y1 = yh[m2];
                   x2 = xsect(m3,m1);
                   y2 = ysect(m3,m1);
                   break;
               case 6: /* Line between vertex 3 and side 1-2 */
                   x1 = xh[m1];
                   y1 = yh[m1];
                   x2 = xsect(m1,m2);
                   y2 = ysect(m1,m2);
                   break;
               case 7: /* Line between sides 1-2 and 2-3 */
                   x1 = xsect(m1,m2);
                   y1 = ysect(m1,m2);
                   x2 = xsect(m2,m3);
                   y2 = ysect(m2,m3);
                   break;
               case 8: /* Line between sides 2-3 and 3-1 */
                   x1 = xsect(m2,m3);
                   y1 = ysect(m2,m3);
                   x2 = xsect(m3,m1);
                   y2 = ysect(m3,m1);
                   break;
               case 9: /* Line between sides 3-1 and 1-2 */
                   x1 = xsect(m3,m1);
                   y1 = ysect(m3,m1);
                   x2 = xsect(m1,m2);
                   y2 = ysect(m1,m2);
                   break;
               default:
                   break;
               }

               /* Finally draw the line */
               /* ConrecLine(x1,y1,x2,y2,z[k]); */
               glVertex2f(x1 * ratio->x, y1 * ratio->y);
               glVertex2f(x2 * ratio->x, y2 * ratio->y);
            } /* m */
         } /* k - contour */
      } /* i */
   } /* j */
   glEnd();
}


