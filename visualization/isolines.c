#include "isolines.h"
#include "../simulation.h"
#include "../visualization.h"
#include <rfftw.h>
#include <math.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void _draw_isoline_point(Visualization *v, Vector *start);
void _draw_point(Visualization *v, Vector *point);
void _draw_isoline_value(Visualization *v, float value);

typedef struct {
  int num_values, num_points;
  float *values;
  Vector **points;
} Isolines_datapoints;

void
isolines_draw(Visualization *v) {
  Isolines_datapoints *data;

  if (v->isolines_datapoints == NULL) {
    data = malloc(sizeof(Isolines_datapoints));
    v->isolines_datapoints = data;

    data->num_values = 0;
    data->num_points = 0;
    data->values = NULL;
    data->points = NULL;

  } else data = v->isolines_datapoints;

  int num = v->isolines_number;

  if (v->isolines_type == VIZ_ISO_BY_VALUE) {

    if (data->num_values != num) {
      if (data->values != NULL) free(data->values);

      float *values;
      values = (float *) malloc(num * sizeof(float));

      int val = num;
      float value = 7.0;
      while (val>0) {
        values[--val] = value;
        value *= 1.5 / num;
      }

      data->values = values;
      data->num_values = num;
    }

    isolines_draw_by_value(v, data->values, data->num_values);
  
  } else if (v->isolines_type == VIZ_ISO_BY_NUM) {

    isolines_draw_by_number(v, num);

  } else if (v->isolines_type == VIZ_ISO_BY_POINT) {

    num /= 2;

    if (data->num_points != num) {

      Vector **values;
      values = (Vector **) malloc(num * sizeof(Vector*));

      int val = num;
      int dim = v->simulation->dimension;
      int jump = pow(dim, 2) / num;
      int loc = 0, x,y;
      while (val>0) {
        loc += jump + (int)(rand() % 100);
        x = loc % dim;
        y = (loc / dim) % dim;
        values[--val] = new_vector(x, y);
      }

      Vector **oldvalues = data->points;
      int oldnum = data->num_points;

      data->points = values;
      data->num_points = num;

      if (oldvalues != NULL) {
        while (oldnum>0) 
          del_vector(oldvalues[--oldnum]);
        free(oldvalues);
      }

    }

    isolines_draw_by_point(v, data->points, data->num_points);
 
  }

}

void
isolines_draw_by_number(Visualization *v, int num) {
  Simulation_statistics *st = simulation_statistics(v->simulation);
  float max = st->max;
  //float min = st->min;
  float mean = st->mean;
  float stepsize = log(max/mean) / num;
  int i = num - 1;

  float *values;
  values = (float*) malloc(num * sizeof(float));

  // printf("max mean min: %f %f %f\nstepsize: %f\n", max, mean, min, stepsize);
  // printf("values: ");
  values[i] = max;
  // printf("%f ", values[i]);
  while (i>0) {
    values[--i] = values[i+1] * stepsize;
    // printf("%f ", values[i]);
  }
  // printf("\n");

  isolines_draw_by_value(v, values, num);
  free(values);
}

void
isolines_draw_by_value(Visualization *v, float *values, int num) {
  while (num>0)
    _draw_isoline_value(v, values[--num]);
}

void
isolines_draw_by_point(Visualization *v, Vector **points, int num) {
  while (num>0) {
    _draw_point(v, points[--num]);
    _draw_isoline_point(v, points[num]);
  }
}

void
_draw_isoline_point(Visualization *v, Vector *start) {
  Simulation *s = v->simulation;
  _draw_isoline_value(v, simulation_value(s, start->x, start->y));
}

void
_draw_point(Visualization *v, Vector *point) {
  glPointSize(10.0);
  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_BLEND);
  glEnable(GL_POINT_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POINTS);
  glVertex2f((v->ratio)->x * point->x, (v->ratio)->y * point->y);
  glEnd();
}

void
_draw_isoline_value(Visualization *v, float value) {
  Simulation *s = v->simulation;
#define VAL(x,y) simulation_value(s, x, y)
#define xsect(p1,p2) (h[p2]*xh[p1]-h[p1]*xh[p2])/(h[p2]-h[p1])
#define ysect(p1,p2) (h[p2]*yh[p1]-h[p1]*yh[p2])/(h[p2]-h[p1])

  int dim = s->dimension;
  Vector *ratio = v->ratio;

  float min, max;

  int x, y, m;

  int sh[5];
  float h[5], xh[5], yh[5];
  int xm[4] = {0,1,1,0}, ym[4]={0,0,1,1};

  int castab[3][3][3] = { { {0,0,8},{0,2,5},{7,6,9} },
                          { {0,3,4},{1,3,1},{4,3,0} },
                          { {9,6,7},{5,2,0},{8,0,0} } };

  glLineWidth(1);
  glBegin(GL_LINES);
  visualization_set_color_palette(v, pow(value, 0.2));

  for (x = 0; x < dim - 1; x++) {
    for (y = 0; y < dim - 1; y++) {

      min = MIN(MIN(VAL(x,y), VAL(x,y+1)), MIN(VAL(x+1,y), VAL(x+1,y+1)));
      max = MAX(MAX(VAL(x,y), VAL(x,y+1)), MAX(VAL(x+1,y), VAL(x+1,y+1)));

      // Quad lies entirely above or under the value
      if (max < value || min > value) continue;

      for (m = 4; m >= 0; m--) {
        if (m > 0) {
          h[m] = VAL(x+xm[m-1], y+ym[m-1]) - value;
          xh[m] = x + xm[m-1];
          yh[m] = y + ym[m-1];
        } else {
          h[m] = 0.25 * (h[1] + h[2] + h[3] + h[4]);
          xh[0] = x + 0.5;
          yh[0] = y + 0.5;
        }
        if (h[m] > 0.0)
          sh[m] = 2;
        else if (h[m] < 0.0)
          sh[m] = 0;
        else
          sh[m] = 1;

      }
      
      /* 
        p1 +-------------------+ p2
           | \               / |
           |   \    m=1    /   |
           |     \       /     |
           |       \   /       |
           |  m=4    X   m=2   |   the centre is p0
           |       /   \       |
           |     /       \     |
           |   /    m=3    \   |
           | /               \ |
        p4 +-------------------+ p3

       p1: current location
       p2: point right of current
       p3: other corner of quad
       p4: point below current
       p0: the point right in the middle of the other four

      */

      for (m = 1; m <= 4; m++) {
        int m1, m2, m3;
        float x1, x2, y1, y2;

        m1 = m;
        m2 = 0;
        m3 = (m % 4) + 1;

        int case_value;
        
        if ((case_value = castab[sh[m1]][sh[m2]][sh[m3]]) == 0) continue;

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

        glVertex2f(x1 * ratio->x, y1 * ratio->y);
        glVertex2f(x2 * ratio->x, y2 * ratio->y);
       
      }

    }
  }
  glEnd();
}

