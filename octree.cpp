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
#include "camera.h"
#include "octree.h"
#include "terrain.h"

/** DEPRECATED **/

static bool octree_free;
static int octree_cnt = 0;

octree_t *octree_root_get (terrain_t *t)
{
	return (octree_t *) t->root;
}

octree_t *octree_node_alloc (terrain_t *t, octree_t *parent, int id)
{
	if (parent) {
		if (parent->children[id])
			return 0;
	}
	
	octree_t *node = (octree_t *) calloc (sizeof (octree_t), 1);
	
	if (parent) {
		node->size_x = parent->size_x / 2;
		node->size_y = parent->size_y / 2;
		node->size_z = parent->size_z / 2;

		node->origin_x = parent->origin_x - node->size_x / 2;
		node->origin_y = parent->origin_y - node->size_y / 2;
		node->origin_z = parent->origin_z - node->size_z / 2;

		switch (id) {
			case 0:
				break;
			case 1:
				node->origin_x += node->size_x;
				break;
			case 2:
				node->origin_y += node->size_y;
				break;
			case 3:
				node->origin_x += node->size_x;
				node->origin_y += node->size_y;
				break;
			case 4:
				node->origin_z += node->size_z;
				break;
			case 5:
				node->origin_x += node->size_x;
				node->origin_z += node->size_z;
				break;
			case 6:
				node->origin_y += node->size_y;
				node->origin_z += node->size_z;
				break;
			case 7:
				node->origin_x += node->size_x;
				node->origin_y += node->size_y;
				node->origin_z += node->size_z;
				break;
		}

		parent->children[id] = node;
	} else {	
		node->size_x = t->dim_x;
		node->size_y = t->dim_y;
		node->size_z = t->dim_z;

		node->origin_x = node->size_x / 2;
		node->origin_y = node->size_y / 2;
		node->origin_z = node->size_z / 2;
	}

	octree_cnt ++;
	//printf ("%d: %d %d %d, %d %d %d, %d\n", cnt, node->origin_x, node->origin_y, node->origin_z, node->size_x, node->size_y, node->size_z, id);
	
	node->children = (octree_t **) calloc (sizeof (octree_t *), 8);
	
	/*for (int i = 0; i < 8; i ++)
		node->children[i] = NULL;*/
		
	node->parent = parent;

	
	//node->children = (octree_t *) malloc (sizeof (octree_t) * 8);
}

void octree_build_children (terrain_t *terrain, octree_t *parent, int id)
{
	

	octree_t *node = octree_node_alloc (terrain, parent, id);

	/* continue until node exist */
	if (!node)
		return;
	
	
	
	octree_build (terrain, node);
	
	
}

void octree_build (terrain_t *terrain, octree_t *parent)
{
	int o_x = parent->size_x / 2;		
	int o_y = parent->size_y / 2;
	int o_z = parent->size_z / 2;	

	if (o_x == 0 || o_y == 0 || o_z == 0)
		return;

	if (!terrain->data || !parent)
		return;
	
		for (int x = 0; x < terrain->dim_x; x ++) {
			for (int y = 0; y < terrain->dim_y; y ++) {
				for (int z = 0; z < terrain->dim_z; z ++) {
					if (octree_free)
						return;
					
					voxel_t *v = terrain_voxel_get (terrain, x, y, z);
										
					if (v->value <= 0.0f)
						continue;
					
					/* Nachazi se voxel uvnitr bounding boxu konkretniho rodice? */
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (terrain, parent, 0);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (terrain, parent, 1);
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (terrain, parent, 2);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (terrain, parent, 3);
						
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (terrain, parent, 4);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (terrain, parent, 5);
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (terrain, parent, 6);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (terrain, parent, 7);
					
				}
			}
		}
}

int octree_thread (void *unused)
{	
	int size_v = TERRAIN_DIM*7;
	
	camera_t *c = camera_get ();

	int a = 0;
	
	/*terrain_t *t = terrain_list.next;
	t->root = octree_node_alloc (NULL, 0);
				
	octree_build (t, (octree_t *) t->root);
	
	t = terrain_add (-32, 0, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);*/
	
	/*t = terrain_add (0, 32, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);
	
	t = terrain_add (32, 32, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);*/
	
	while (1) {

		//	a = 0;
			
			/*octree_t *node = (octree_t *) t->root;
			
			if (!node) {
				
					node = octree_node_alloc (t, NULL, 0);
					
					t->root = (void *) node;
					
					octree_build (t, node);
					
				//	SDL_UnlockMutex (gl_mutex);
				//}
			}*/
						
			//bool b = false;
			
			/*for (int y = -size_v; y < size_v; y ++) {
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
			}*/
			
			//printf ("- %f .. %f .. %f\n", c->pos_x, c->pos_y, c->pos_z);
			

	
				
				
					
/*				int cx = -((int) (c->pos_x) / d) * d;
				int cy = -((int) (c->pos_y) / d) * d;
				int cz = -((int) (c->pos_z) / d) * d;
					
				if (c->state & CAMERA_STATE_MOUSECLICK) {
					if (t->origin_x == cx && t->origin_y == cy && t->origin_z == cz) {
						//printf (": %d %d - %d %d\n", t->origin_x, t->origin_y, -cx, -cy);
						//printf (": %d %d - %d %d\n", t->origin_x, t->origin_y, -cx, -cy);
						float bx = c->pos_x+(float) t->origin_x;
						float by = c->pos_y+(float) t->origin_y;
						float bz = c->pos_z+(float) t->origin_z;
						
						if (bx < 0)
							bx += 32;
						if (by < 0)
							by += 32;
						if (bz < 0)
							bz += 32;
				
						bx = 32 - bx;
						by = 32 - by;
						bz = 32 - bz;
						
						voxel_t *v;
						
						bool v_c = false;
						
						int v_size = 3;
						
						for (int i_x = -v_size; i_x < v_size; i_x ++)
						for (int i_y = -v_size; i_y < v_size; i_y ++)
						for (int i_z = -v_size; i_z < v_size; i_z ++) {
							v = terrain_voxel_get (t, (int) bx+i_x, (int) by+i_y, (int) bz+i_z);
							
							if (v->value >= 0) {
								v->value = -1;
								v_c = true;
							}
						}
						
						printf ("v: %f %f %f\n", v->value, bx, by);			

						if (v_c)
							gl_prepare_terrain (t);
					
						c->state &= ~CAMERA_STATE_MOUSECLICK;
					}
				}*/
			//}
			
			/*if (((node->origin_x-size_v) < -c->pos_x/TERRAIN_VOXEL_DIM) &&
				((node->origin_x+size_v) > -c->pos_x/TERRAIN_VOXEL_DIM) &&
				((node->origin_y-size_v) < -c->pos_y/TERRAIN_VOXEL_DIM) &&
				((node->origin_y+size_v) > -c->pos_y/TERRAIN_VOXEL_DIM) &&
				((node->origin_z-size_v) < -c->pos_z/TERRAIN_VOXEL_DIM) &&
				((node->origin_z+size_v) > -c->pos_z/TERRAIN_VOXEL_DIM))
				a ++;
		
			if (!a) {
				printf ("chybi!\n");
				int d = TERRAIN_DIM;
				
				int x = -((int) (c->pos_x / 2 + d) / d) * d;
				int y = -((int) (c->pos_y / 2 + d) / d) * d;
				int z = -((int) (c->pos_z / 2) / d) * d;
				
				if (SDL_LockMutex (gl_mutex) == 0) {
					printf ("pridavam teren: %d %d %d\n", x, y, z);
					terrain_add (x, y, 0);
					
					SDL_UnlockMutex (gl_mutex);
				}
				
				//break;
			}*/
			
			
		//}
		
		SDL_Delay (1);
	}
	
	return 0;
}

int octree_thread2 (void *unused)
{	
	int size_v = TERRAIN_DIM*8;
	
	camera_t *c = camera_get ();

	int a = 0;
	
	/*terrain_t *t = terrain_list.next;
	t->root = octree_node_alloc (NULL, 0);
				
	octree_build (t, (octree_t *) t->root);
	
	t = terrain_add (-32, 0, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);*/
	
	/*t = terrain_add (0, 32, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);
	
	t = terrain_add (32, 32, 0);
	t->root = octree_node_alloc (NULL, 0);	
	octree_build (t, (octree_t *) t->root);*/
	
/*	while (1) {
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
	}*/
	
	return 0;
}

void octree_deinit_node (octree_t *node)
{
	if (!node)
		return;
	
	terrain_t *t = terrain_get ();

	for (int i = 0; i < 8; i ++) {
		if (node->children[i]) {
			octree_deinit_node (node->children[i]);
		}
	}
	
	//node->parent = NULL;
	free (node->children);
	free (node);
}

bool octree_deinit ()
{
	/* je nutne ukoncit sestavovani OcTree, abychom mohli uvolnit teren */
	octree_free = true;
	
	SDL_Delay (1);
	
	terrain_t *t;
	for (t = terrain_list.next; t != &terrain_list; t = t->next) {
		octree_deinit_node ((octree_t *) t->root);
	}
	
	return true;
}

bool octree_init ()
{
	octree_free = false;
	
	return true;
}
