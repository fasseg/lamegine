/*
 * lamegine.c
 *
 *  Created on: Feb 1, 2013
 *      Author: ruckus
 */
#include "lamegine.h"

Vector3f velocity = { .x = 0.0, .y = 0.0, .z = 0.0 };
Vector3f camera = { .x = 0.0, .y = 0.0, .z = 0.0 };
Vector3f los = { .x = 0.0, .y = 0.0, .z = 1.0 };
Vector3f world_up = { .x = 0.0, .y = 1.0, .z = 0.0 };

int last_mouse_pos_x;
float smooth_rot_x;
int movement_flags = 0;
double last_jump_ts = 0.0;

int main(char** argv, int argc) {
	Display *dsp = XOpenDisplay(NIL);
	assert(dsp);

	int colorBlack = BlackPixel(dsp, DefaultScreen(dsp)) ;
	int colorWhite = WhitePixel(dsp, DefaultScreen(dsp)) ;

	Window root = DefaultRootWindow(dsp) ;

	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo *vi = glXChooseVisual(dsp, 0, att);

	// check if visual is available
	if (vi == NULL ) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0);
	} else {
		printf("\n\tvisual %p selected\n", (void *) vi->visualid);
	}

	Colormap cmap = XCreateColormap(dsp, root, vi->visual, AllocNone);

	XSetWindowAttributes xsAttrs;
	xsAttrs.colormap = cmap;
	xsAttrs.event_mask = ExposureMask | KeyPressMask;

	Window wnd = XCreateWindow(dsp, root, 0, 0, WIDTH, HEIGHT, 0, vi->depth,
			InputOutput, vi->visual, CWColormap | CWEventMask, &xsAttrs);
	XMapWindow(dsp, wnd);
	XStoreName(dsp, wnd, "lamegine v0.0.1");

	XWindowAttributes winAttrs;
	int screenNumber = DefaultScreen(dsp);

	GLXContext glx = glXCreateContext(dsp, vi, NULL, GL_TRUE);
	glXMakeCurrent(dsp, wnd, glx);
	glEnable(GL_DEPTH_TEST);

	// disable auto repeat
	Bool detectableAutoRepeatSupported;
	XkbSetDetectableAutoRepeat(dsp, True, &detectableAutoRepeatSupported);
	printf("detectable autorepeat supported: %d\n",
			detectableAutoRepeatSupported);

	// registering listener for interesting events
	XSelectInput(dsp, wnd,
			KeyPressMask | KeyReleaseMask | ExposureMask | PointerMotionMask);

	Bool exposed = False;

	// init alsa audio sub system
	// init_sound();
	printf("\ninitialization done\n");

	for (;;) {
		XEvent e;
		int code;
		if (XCheckIfEvent(dsp, &e, check_event, "1") == True) {
			if (e.type == Expose) {
				exposed = True;
				XGetWindowAttributes(dsp, wnd, &winAttrs);
				glViewport(0, 0, winAttrs.width, winAttrs.height);
				// and query the mouse cursor for the original position
				int root_x, root_y, last_mouse_pos_y;
				unsigned int ret_mask;
				Window child_ret;
				XQueryPointer(dsp, wnd, &root, &child_ret, &root_x, &root_y,
						&last_mouse_pos_x, &last_mouse_pos_y, &ret_mask);
				render_scene();
				glXSwapBuffers(dsp, wnd);
			} else if (e.type == KeyPress || e.type == KeyRelease) {
				code = XLookupKeysym(&e.xkey, 0);
				switch (code) {
				case XK_Escape:
					glXMakeCurrent(dsp, None, NULL );
					glXDestroyContext(dsp, glx);
					XDestroyWindow(dsp, wnd);
					XCloseDisplay(dsp);
					exit(0);
					break;
				case XK_w:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_MOVE_FORWARD;
					} else {
						movement_flags &= ~FLAG_MOVE_FORWARD;
					}
					break;
				case XK_s:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_MOVE_BACKWARDS;
					} else {
						movement_flags &= ~FLAG_MOVE_BACKWARDS;
					}
					break;
				case XK_d:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_MOVE_RIGHT;
					} else {
						movement_flags &= ~FLAG_MOVE_RIGHT;
					}
					break;
				case XK_a:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_MOVE_LEFT;
					} else {
						movement_flags &= ~FLAG_MOVE_LEFT;
					}
					break;
				case XK_q:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_ROTATE_LEFT;
					} else {
						movement_flags &= ~FLAG_ROTATE_LEFT;
					}
					break;
				case XK_e:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_ROTATE_RIGHT;
					} else {
						movement_flags &= ~FLAG_ROTATE_RIGHT;
					}
					break;
				case XK_space:
					if (e.type == KeyPress) {
						movement_flags |= FLAG_JUMP;
					}
					break;
				default:
					break;
				}
			} else if (e.type == MotionNotify) {
				int px = e.xmotion.x - last_mouse_pos_x;
				last_mouse_pos_x = e.xmotion.x;
				smooth_rot_x += px;
			}
		} else if (exposed == True) {
			camera.x = camera.x + velocity.x;
			render_scene();
			glXSwapBuffers(dsp, wnd);
			char v_msg[20];
			sprintf(v_msg, "Vel (%.2f %.2f %.2f)", velocity.x, velocity.y,
					velocity.z);
			char dir_msg[20];
//			sprintf(dir_msg, "Angle: %f", anglex);
			char pos_msg[20];
			sprintf(pos_msg, "Pos (%.2f %.2f %.2f)", camera.x, camera.y,
					camera.z);
			char los_msg[20];
			sprintf(los_msg, "Los (%.2f %.2f %.2f)", los.x, los.y, los.z);
			XColor fg;
			char colorString[] = "#00ff00";
			GC textGC = XCreateGC(dsp, wnd, 0, 0);
			XParseColor(dsp, cmap, colorString, &fg);
			XAllocColor(dsp, cmap, &fg);
			XSetForeground(dsp, textGC, fg.pixel);
			XDrawString(dsp, wnd, textGC, 5, 15, v_msg, strlen(v_msg));
//			XDrawString(dsp, wnd, textGC, 5, 30, dir_msg, strlen(dir_msg));
			XDrawString(dsp, wnd, textGC, 5, 45, pos_msg, strlen(pos_msg));
			XDrawString(dsp, wnd, textGC, 5, 60, los_msg, strlen(los_msg));
		}

	}
	return 0;
}
void render_scene() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 1.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	set_vector_null(&velocity);
	if ((movement_flags & FLAG_MOVE_FORWARD) == FLAG_MOVE_FORWARD) {
		add(&velocity, los);
	}
	if ((movement_flags & FLAG_MOVE_BACKWARDS) == FLAG_MOVE_BACKWARDS) {
		subtract(&velocity, los);
	}
	if ((movement_flags & FLAG_MOVE_RIGHT) == FLAG_MOVE_RIGHT) {
		Vector3f cross = cross_product(los, world_up);
		add(&velocity, cross);
	}
	if ((movement_flags & FLAG_MOVE_LEFT) == FLAG_MOVE_LEFT) {
		Vector3f cross = cross_product(world_up, los);
		add(&velocity, cross);
	}
	if ((movement_flags & FLAG_ROTATE_RIGHT) == FLAG_ROTATE_RIGHT) {
		rotate_y(&los, -3);
	}
	if ((movement_flags & FLAG_ROTATE_LEFT) == FLAG_ROTATE_LEFT) {
		rotate_y(&los, 3);
	}

	if ((movement_flags & FLAG_JUMP) == FLAG_JUMP) {
		if (last_jump_ts == 0.0){
			last_jump_ts = current_ms();
		}
		double current = current_ms();
		double t = current - last_jump_ts - 250;
		velocity.y = 2.0*t/500.0;
		if (t >= 250.0){
			last_jump_ts = 0.0;
			movement_flags &= ~FLAG_JUMP;
		}
	}

	// smooth look via mouse
	if (smooth_rot_x > 1) {
//		float step = 1.0 + smooth_rot_x / 4.0;
		float step = smooth_rot_x;
		smooth_rot_x = smooth_rot_x - step;
		rotate_y(&los, -step);
	} else if (smooth_rot_x < -1) {
		float step = smooth_rot_x;
//		float step = -1.0 + smooth_rot_x / 4.0;
		smooth_rot_x = smooth_rot_x - step;
		rotate_y(&los, -step);
	}

// calculate the angle between the LOS vector and the z axis
// since the whole thing will be rotated by this angle
	float angle_y;
	if (los.z >= 0) {
		angle_y = asin(los.x) * 180 / M_PI;
	} else if (los.z < 0) {
		angle_y = 180 - (asin(los.x) * 180 / M_PI);
	}
	glRotatef(-angle_y, 0.0, 1.0, 0.0);

	if (magnitude(velocity) > 0.0) {
		normalize(&velocity);
		scale(&velocity, 0.1);
		add(&camera, velocity);
		if (camera.y > 0.0){
			camera.y = 0.0;
		}
	}
	glTranslatef(camera.x, camera.y, camera.z);

	Vector3f pos1 = (Vector3f )
			{ .x = 0.0, .y = 0.0, .z = 0.0 };
	Box b1 = create_box(pos1, 3.0, 3.0, 3.0);

	Vector3f pos2 = (Vector3f )
			{ .x = 10.0, .y = 0.0, .z = 10.0 };
	Box b2 = create_box(pos2, 1.0, 1.0, 1.0);

	Vector3f pos3 = (Vector3f )
			{ .x = -10.0, .y = 0.0, .z = 10.0 };
	Box b3 = create_box(pos3, 1.0, 1.0, 1.0);

	Vector3f pos4 = (Vector3f )
			{ .x = -10.0, .y = 0.0, .z = -10.0 };
	Box b4 = create_box(pos4, 1.0, 1.0, 1.0);

	Vector3f pos5 = (Vector3f )
			{ .x = 10.0, .y = 0.0, .z = -10.0 };
	Box b5 = create_box(pos5, 1.0, 1.0, 1.0);

	render_box(b1);
	render_box(b2);
	render_box(b3);
	render_box(b4);
	render_box(b5);
	render_floor();
}

void render_box(Box b) {
	glBegin(GL_QUADS);
// back face
	glColor3f(0.5, 1.0, 1.0);
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
// right face
	glColor3f(1.0, 0.5, 1.0);
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
// left face
	glColor3f(1.0, 1.0, 0.5);
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
// top face
	glColor3f(1.0, 0.5, 0.5);
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
// bottom face
	glColor3f(0.5, 0.5, 1.0);
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z - 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
// front face
	glColor3f(0.5, 1.0, 0.5);
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y + (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x + (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glVertex3f(b.pos.x - (0.5 * b.width), b.pos.y - (0.5 * b.height),
			b.pos.z + 0.5 * (b.depth));
	glEnd();
}

void render_floor() {
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUADS);
	glVertex3f(-10.0, -1.0, -10.0);
	glVertex3f(10.0, -1.0, -10.0);
	glVertex3f(10.0, -1.0, 10.0);
	glVertex3f(-10.0, -1.0, 10.0);
	glEnd();
}

// a procedure to catch events without blocking
Bool check_event(Display *dsp, XEvent *e, char *arg) {
	int code;
	switch (e->type) {
	case Expose:
		return True;
	case KeyPress:
		return True;
	case KeyRelease:
		return True;
	case MotionNotify:
		return True;
	default:
		return False;
	}
}

// create and return an empty box shape
Box create_box(Vector3f pos, float width, float height, float depth) {
	Box b = { .type = SHAPE_BOX, .pos = pos, .width = width, .height = height,
			.depth = depth };
	return b;
}

double current_ms(){
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
}
