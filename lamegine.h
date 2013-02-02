/*
 * lamegine.h
 *
 *  Created on: Feb 1, 2013
 *      Author: ruckus
 */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <time.h>
#include <pulse/pulseaudio.h>
#include "vectors.h"
#include "audio.h"

#ifndef LAMEGINE_H_
#define LAMEGINE_H_

#define NIL (0)

static const int SHAPE_BOX = 1;
static const int FLAG_MOVE_FORWARD = 1;
static const int FLAG_MOVE_BACKWARDS = 2;
static const int FLAG_MOVE_LEFT = 4;
static const int FLAG_MOVE_RIGHT = 8;
static const int FLAG_ROTATE_LEFT = 16;
static const int FLAG_ROTATE_RIGHT = 32;
static const int FLAG_JUMP = 64;
static const int WIDTH = 1024;
static const int HEIGHT = 768;

typedef struct {
	int type;
	Vector3f pos;
	Vector3f face;
	float width;
	float height;
	float depth;
} Box;

void render_scene();
void render_box(Box b);
void render_floor();
Box create_box(Vector3f pos, float width, float height, float depth);
Bool check_event(Display *, XEvent *, char* arg);
double current_ms();

#endif /* LAMEGINE_H_ */
