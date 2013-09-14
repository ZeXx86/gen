#ifndef __camera_h__
#define __camera_h__

#define CAMERA_SPEED	1.0f / 50

typedef struct {
	float pos_x, pos_y, pos_z;

	float rot_x, rot_y;
} camera_t;

extern camera_t *camera_get ();
extern bool camera_init ();

#endif

