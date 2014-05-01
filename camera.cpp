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

#include "gen.h"
#include "camera.h"

static camera_t cam;

camera_t *camera_get ()
{
	return &cam;
}

void camera_update ()
{
	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();
	glViewport (cam.viewport_x, cam.viewport_y, cam.window_width, cam.window_height);
	
	cam.view = glm::rotate (cam.rot_y, glm::vec3 (1, 0, 0)) * glm::rotate (cam.rot_x, glm::vec3 (0, 1, 0)) * glm::translate (cam.pos);

	cam.MVP = cam.projection * cam.view * cam.model;
	
	glLoadMatrixf (glm::value_ptr (cam.MVP));
}

bool camera_init (int viewport_x, int viewport_y, int window_width, int window_height)
{
	cam.viewport_x = viewport_x;
	cam.viewport_y = viewport_y;
	cam.window_width = window_width;
	cam.window_height = window_height;
	
	cam.aspect = double (window_width) / double (window_height);
	
	cam.fov = 60;
	
	// Projection matrix : 60° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	cam.projection = glm::perspective (cam.fov, (float) cam.aspect, NEAR_PLANE, FAR_PLANE);
 	cam.pos        = glm::vec3 (0, -80, 0);
	cam.model      = glm::mat4 (1.0f);  // Changes for each model !
	
	return true;
}

void camera_deinit ()
{
	
}
