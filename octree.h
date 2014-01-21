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

#ifndef __octree_h__
#define __octree_h__

#include "terrain.h"

typedef struct octree {
	int origin_x, origin_y, origin_z;
	
	int size_x, size_y, size_z;
	
	octree *parent;
	octree **children;
} octree_t;

extern octree_t *octree_root_get ();
extern void octree_build (terrain_t *terrain, octree_t *parent);
extern bool octree_deinit ();
extern bool octree_init ();

#endif

