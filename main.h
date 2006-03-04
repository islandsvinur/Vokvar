#ifndef MAIN_H
#define MAIN_H

#include "global.h"

#define MDEBUG(...) DEBUG("main", __VA_ARGS__)

#include "simulation.h"
#include "visualization.h"

void main_stop(void);

#endif /* MAIN_H */
