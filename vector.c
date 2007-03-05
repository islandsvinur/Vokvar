
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "vector.h"

Vector *
new_vector(float x, float y) {
  Vector *v;
 
  v = malloc(sizeof(Vector));
  assert(v != NULL);

  /* Initialize vector with params */
  v->x = x;
  v->y = y;

  return v;
}

void
del_vector(Vector *v) {
  free(v);
}

Vector *copy_vector (Vector *oldvector) {
 // assert(oldvector != NULL);
  if (oldvector == NULL)
		return NULL;
	else
    return new_vector(oldvector->x, oldvector->y);
}

inline int vectors_equal(Vector *v1, Vector *v2) {
  return (v1->x == v2->x) && (v1->y == v2->y);
}

float vector_dist(Vector *v1, Vector *v2) {
	if (v1 == NULL || v2 == NULL)
		return -1;
	else
		return hypot((v1->x - v2->x), (v1->y - v2->y));
}

Vector * vector_scal_mul(Vector *v, float l) {
	if (v == NULL)
		return NULL;
	else
		return new_vector(v->x * l, v->y * l);
}

Vector * vector_mul(Vector *v1, Vector *v2) {
	if (v1 == NULL || v2 == NULL)
		return NULL;
	else
		return new_vector(v1->x * v2->x, v1->y * v2->y);
}

Vector * vector_add(Vector *v1, Vector *v2) {
	return new_vector(v1->x + v2->x, v1->y + v2->y);
}

int vector_normalize(Vector *v, int dimension) {
  Vector *cp = copy_vector(v);

  while (v->x > dimension) v->x -= dimension;
  while (v->x < 0) v->x += dimension;
  while (v->y > dimension) v->y -= dimension;
  while (v->y < 0) v->y += dimension;

  int result = (cp->x != v->x || cp->y != v->y);

  free(cp);

  return result;
}

