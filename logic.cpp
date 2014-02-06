/*
 *  (C) Copyright 2014 ZeXx86 (tomasj@spirit-system.com)
 *  (C) Copyright 2013 ZeXx86 (tomasj@spirit-system.com)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "gl.h"
#include "gen.h"
#include "kbd.h"
#include "mouse.h"
#include "logic.h"
#include "camera.h"
#include "terrain.h"

unsigned char kdb_pressed = 0;
unsigned char mouse_state = 0;

bool logic_collision (camera_t *p, float x, float y)
{

	return false;
}

void logic_motion (camera_t *c)
{
	/* MOUSE MOTION */
	mouse_t *m = mouse_get ();
	
	/* realizace rotace kamery */
	c->rot_dx += m->x / 3.0f;
	c->rot_dy += m->y / 3.0f;
	
	if (c->rot_dy > 90)
		c->rot_dy = 90;
	else if (c->rot_dy < -90)
		c->rot_dy = -90; 
	
	if (c->rot_dx > 360) {
		c->rot_dx -= 360;
		c->rot_x -= 360;
	} else if (c->rot_dx <= 0) {
		c->rot_dx += 360;
		c->rot_x += 360;
	}

	
	float speed = 0;

#ifdef ANDROID
	speed = m->abs_y < 13000 ? 0.5f : (m->abs_y > 20000 ? -0.5f : 0);
#else
	if (kbd_key_pressed (SDLK_a))
		c->rot_dx -= 0.5f;
	if (kbd_key_pressed (SDLK_d))
		c->rot_dx += 0.5f;
	if (kbd_key_pressed (SDLK_w))
		speed = 1;
	if (kbd_key_pressed (SDLK_s))
		speed = -1;
#endif
	/* vyhlazovaci algoritmus rotace */
	c->rot_x += 0.02f * (c->rot_dx - c->rot_x);
	c->rot_y += 0.02f * (c->rot_dy - c->rot_y);

	//Vector v (c->rot_x/180-1, c->rot_y/90, 0);
	//v.Normalize ();
	
	//printf ("v: %f %f\n", v.GetX(), v.GetY());
	
	/* DEPRECATED - specialni klavesy */
	if (!kdb_pressed) {
		if (kbd_key_pressed (SDLK_q)) {
			if (c->view_dist != 2)
				c->view_dist /= 2;
			else
				c->view_dist = 32;

			kdb_pressed = 255;
		}
	} else
		kdb_pressed --;
	
	
	/* pohyb kamery v prostoru */
	c->pos_x += sinf (M_PI/180 * -c->rot_x) * cosf (M_PI/180 * c->rot_y) * (speed * CAMERA_SPEED);
	c->pos_y += cosf (M_PI/180 * -c->rot_x) * cosf (M_PI/180 * c->rot_y) * (speed * CAMERA_SPEED);	
	c->pos_z += sinf (M_PI/180 * c->rot_y) * (speed * CAMERA_SPEED);

	/* omezeni vstupu do vody */
	if (c->pos_z > -3)
		c->pos_z = -3;
	
	/* zmena stavu kamery na zaklade stavu mysi */
	if (m->state && !c->state) {
		//mouse_state
		if (m->state & CAMERA_STATE_LMOUSECLICK)
			c->state |= CAMERA_STATE_LMOUSECLICK;
		if (m->state & CAMERA_STATE_RMOUSECLICK)
			c->state |= CAMERA_STATE_RMOUSECLICK;
	}
}

int logic_thread (void *unused)
{
	camera_t *c = camera_get ();
	
	for (;;) {
		logic_motion (c);

		SDL_Delay (1);
	}

	return 0;
}

static float contact (float x, float y, float z, float x2, float y2, float z2)
{
	return sqrtf ((x-x2)*(x-x2) + (y-y2)*(y-y2));	
}


int login_thread_tadd (void *unused)
{	
	int size_v = TERRAIN_DIM*7;
	
	camera_t *c = camera_get ();

	while (1) {			
		for (int y = -size_v; y < size_v; y ++) {
			for (int x = -size_v; x < size_v; x ++) {
				int d = TERRAIN_DIM;
					
				int cx = -((int) ((c->pos_x) + x) / d) * d;
				int cy = -((int) ((c->pos_y) + y) / d) * d;
		
				//printf ("%d %d\n", cx, cy);
				//int cz = -((int) (c->pos_z / 2) / d) * d;
					
				if (contact (-c->pos_x, -c->pos_y, 0, cx, cy, 0) <= size_v) {
					terrain_add (cx, cy, 0);
					terrain_add (cx, cy, TERRAIN_DIM);
				}
			}
		}
			
		SDL_Delay (1);
	}
	
	return 0;
}

int logic_thread_tdel (void *unused)
{	
	int size_v = TERRAIN_DIM*8;
	
	camera_t *c = camera_get ();
	
	while (1) {
		terrain_t *t;
		
		if (SDL_TryLockMutex (gl_mutex) == 0) {
			for (t = terrain_list.next; t != &terrain_list; t = t->next) {
				if (contact (-c->pos_x, -c->pos_y, 0, t->origin_x, t->origin_y, 0) > size_v) {
					if (t->gl_buf_len) {
						terrain_del (t);
					}
				}
			}

			SDL_UnlockMutex (gl_mutex);
		}
		
		SDL_Delay (1);
	}
	
	return 0;
}

int logic_thread_mouse (void *unused)
{
	camera_t *c = camera_get ();
	
	while (1) {
		if (c->state & CAMERA_STATE_LMOUSECLICK) {
			terrain_regen (c, -1);
					
			c->state &= ~CAMERA_STATE_LMOUSECLICK;
		} else if (c->state & CAMERA_STATE_RMOUSECLICK) {
			terrain_regen (c, 1);
					
			c->state &= ~CAMERA_STATE_RMOUSECLICK;
		}
		
		SDL_Delay (10);
	}
}

bool logic_init ()
{
	if (!SDL_CreateThread (logic_thread, NULL, NULL)) {
		cerr << "Unable to create thread: " << SDL_GetError () << endl;
		return false;
	}

	if (!SDL_CreateThread (login_thread_tadd, NULL, NULL)) {
		cerr << "Unable to create thread: " << SDL_GetError () << endl;
		return false;
	}

	if (!SDL_CreateThread (logic_thread_tdel, NULL, NULL)) {
		cerr << "Unable to create thread: " << SDL_GetError () << endl;
		return false;
	}

	if (!SDL_CreateThread (logic_thread_mouse, NULL, NULL)) {
		cerr << "Unable to create thread: " << SDL_GetError () << endl;
		return false;
	}
	
	return true;
}

void logic_deinit ()
{
	
}
