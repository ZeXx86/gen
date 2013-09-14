#include "gen.h"
#include "camera.h"

static camera_t camera;

camera_t *camera_get ()
{
	return &camera;
}

bool camera_init ()
{
	camera.pos_x = 6;
	camera.pos_y = 6;
	camera.pos_z = -80;

	camera.rot_x = 135;
	camera.rot_y = 20;

	return true;
}
