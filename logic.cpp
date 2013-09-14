#include "gl.h"
#include "gen.h"
#include "kbd.h"
#include "mouse.h"
#include "logic.h"
#include "camera.h"
#include "terrain.h"


bool logic_collision (camera_t *p, float x, float y)
{

	return false;
}

void logic_motion (camera_t *c)
{
	/* MOUSE MOTION */
	mouse_t *m = mouse_get ();
	
	c->rot_x += m->x / 2;
	c->rot_y += m->y / 2;

	float speed = 0;

	if (kbd_key_pressed (SDLK_LEFT))
		c->rot_x -= 0.5f;
	if (kbd_key_pressed (SDLK_RIGHT))
		c->rot_x += 0.5f;
	if (kbd_key_pressed (SDLK_UP))
		speed = 1;
	if (kbd_key_pressed (SDLK_DOWN))
		speed = -1;

	c->pos_x += sinf (M_PI/180 * -c->rot_x) * (speed * CAMERA_SPEED);
	c->pos_y += cosf (M_PI/180 * -c->rot_x) * (speed * CAMERA_SPEED);	
	c->pos_z += sinf (M_PI/180 * c->rot_y) * (speed * CAMERA_SPEED);
}

int logic_thread (void *unused)
{
	for (;;) {
		camera_t *c = camera_get ();

		logic_motion (c);

		SDL_Delay (1);
	}

	return 0;
}

bool logic_init ()
{
	if (!SDL_CreateThread (logic_thread, NULL)) {
		cerr << "Unable to create thread: " << SDL_GetError () << endl;
		return false;
	}

	return true;
}

void logic_deinit ()
{
	
}
