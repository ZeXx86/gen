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

static camera_t camera;

camera_t *camera_get ()
{
	return &camera;
}

bool camera_init ()
{
	/* nastaveni pocatecni pozice kamery */
	camera.pos_x = 32;
	camera.pos_y = 32;
	camera.pos_z = -40;
	
	/* nastaveni pocatecniho natoceni kamery */
	camera.rot_x = 0;
	camera.rot_y = 0;

	/* specialni parametry */
	camera.view_dist = 8;

	return true;
}
