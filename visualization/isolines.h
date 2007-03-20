#ifndef ISOLINES_H
#define ISOLINES_H

#include "../visualization.h"

void isolines_draw(Visualization *v);
void isolines_draw_by_number(Visualization *v, int num);
void isolines_draw_by_value(Visualization *v, float *values, int num);
void isolines_draw_by_point(Visualization *v, Vector **points, int num);

#endif /* ISOLINES_H */
