#ifndef __camera_h__
#define __camera_h__

#define CAMERA_SPEED			1.0f / 50
#define CAMERA_STATE_LMOUSECLICK	0x1
#define CAMERA_STATE_RMOUSECLICK	0x4

typedef struct {
	float pos_x, pos_y, pos_z;

	float rot_x, rot_y;	// smoothed movement
	float rot_dx, rot_dy;	// direct movement

	unsigned view_dist;
	unsigned state;
} camera_t;

extern camera_t *camera_get ();
extern bool camera_init ();

#endif

