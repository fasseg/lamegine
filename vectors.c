/*
 * vectors.c
 *
 *  Created on: Feb 1, 2013
 *      Author: ruckus
 */
#include "vectors.h"

void rotate_y(Vector3f *vector, float degree) {
	//         | cos 0 sin  |
	// rot_y = |  0  1  0   |
 	//         | -sin 0 cos |
	float rad = degree * M_PI/180.0;
	float x = vector->x * cosf(rad) + vector->z * sinf(rad);
	float z = vector->x * -sinf(rad) + vector->z * cosf(rad);
	vector->x = x;
	vector->z = z;
}

void add(Vector3f *target, Vector3f src){
	target->x = target->x + src.x;
	target->y = target->y + src.y;
	target->z = target->z + src.z;
}

void subtract(Vector3f *target, Vector3f src){
	target->x = target->x - src.x;
	target->y = target->y - src.y;
	target->z = target->z - src.z;
}

void set_vector_null(Vector3f *vector) {
	vector->x = 0.0;
	vector->y = 0.0;
	vector->z = 0.0;
}

float dot_product(Vector3f a, Vector3f b) {
	float dot = a.x * b.x + a.y * b.y + a.z * b.z;
	return dot;
}

void scale(Vector3f *vector, float factor) {
	vector->x = factor * vector->x;
	vector->y = factor * vector->y;
	vector->z = factor * vector->z;
}

Vector3f cross_product(Vector3f a, Vector3f b) {
	Vector3f cross;
	cross.x = a.y * b.z - a.z * b.y;
	cross.y = a.z * b.x - a.x * b.z;
	cross.z = a.x * b.y - a.y * b.x;
	return cross;
}

void normalize(Vector3f *vector) {
	float mag = magnitude(*vector);
	vector->x = vector->x / mag;
	vector->y = vector->y / mag;
	vector->z = vector->z / mag;
}

void print_vector(Vector3f vector, char *title) {
	printf("%s: [%.2f %.2f %.2f] Mag: %.2f\n", title, vector.x, vector.y,
			vector.z, magnitude(vector));
}

float magnitude(Vector3f vector) {
	return sqrtf(
			(vector.x * vector.x) + (vector.y * vector.y)
					+ (vector.z * vector.z));
}
