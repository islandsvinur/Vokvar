/* $Id: vector.h,v 1.7 2003/05/27 09:36:39 cluijten Exp $ */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

/* Vector ADT */

/** Vector
 * Simple vector typing representing a (X, Y) tuple.
 */
typedef struct {
  float x; /**< X coordinate */
  float y; /**< Y coordinate */
} Vector;

/** Creates a vector.
  * \param x X coordinate
  * \param y Y coordinate
  */
Vector * new_vector(float x, float y);

/** Destroys a vector.
  * \param v Pointer to vector to be destroyed.
  */
void del_vector(Vector *v);

/** Checks if given vectors are equal. */
inline int vectors_equal(Vector *v1, Vector *v2);

/** Copies a vector into memory.
 * \param oldvector Pointer to vector to be copied.
 */
Vector * copy_vector(Vector * oldvector);

/** Calculates the distance between two vectors
 * \param v1 and \param v2 are Pointers to vector to be calculated.
 */
float vector_dist(Vector *v1, Vector *v2);

/** Scalar multiplication
 * \param v Pointer to vector to be multiplied
 * \param l Scalar of the multiplication
 */
Vector * vector_scal_mul(Vector *v, float l);

/** Vector multiplication
 * \param v1 and \param v2 Pointers to vectors to be multiplied
 */
Vector * vector_mul(Vector *v1, Vector *v2);

/** Vector addition
 * \param v1 and \param v2 Pointers to vectors to be added
 */
Vector * vector_add(Vector *v1, Vector *v2);

#endif /* VECTOR_H  */
