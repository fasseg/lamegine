/*
 * vectors.h
 *
 *  Created on: Feb 1, 2013
 *      Author: ruckus
 */
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef VECTORS_H_
#define VECTORS_H_

typedef struct{
    float x;
    float y;
    float z;
}Vector3f;

void rotate_y(Vector3f *vector, float degree);
void add(Vector3f *target, Vector3f *src);
float magnitude(Vector3f *vector);
void rotate_los(Vector3f *vector, float angle_y);
void normalize(Vector3f *vector);
Vector3f cross_product(Vector3f *a, Vector3f *b);
void print_vector(Vector3f *vector, char * title);
void set_vector_null(Vector3f *vector);
float dot_product(Vector3f *a, Vector3f *b);
void scale(Vector3f *vector, float factor);
void subtract(Vector3f *target, Vector3f *src);

#endif /* VECTORS_H_ */
