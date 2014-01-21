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
#include "terrain.h"
#include "polygonise.h"

bool frustum_check (terrain_t *t, camera_t *c)
{
	/* FRUSTUM CULLING */
	float f_x = (sinf (M_PI/180 * -c->rot_x) * (t->origin_x+c->pos_x) + cosf (M_PI/180 * -c->rot_x) * (t->origin_y+c->pos_y));/* + sinf (M_PI/180 * (-c->rot_y)) * -c->pos_z;*/
	float f_y = sinf (M_PI/180 * (c->rot_y)) * c->pos_z;
	
	if (0 < f_x)
		return false;
	
	return true;
}

bool frustum_init ()
{
	
	return true;
}