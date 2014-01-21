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
#include "terrain.h"

terrain_t terrain_list;
static voxel_t voxel_null = { .value = 0 };

/* jednoduche matematicke ukony */
static int fastfloor (const float x) 
{
	return x > 0 ? (int) x : (int) x - 1;
}

static float dot (const int *g, const float x, const float y )
{
	return g[0] * x + g[1] * y;
}
static float dot (const int *g, const float x, const float y, const float z)
{
	return g[0] * x + g[1] * y + g[2] * z;
}

/* Implementace 3D raw simplex noise */
float terrain_noise_get (const float x, const float y, const float z)
{
	float n0, n1, n2, n3; // Promenne urcujici prispevek 4 vrcholu

	float skew_f = 1.0f / 3.0f;	// Skew faktor
	float s = (x + y + z) * skew_f;
	
	int i = fastfloor (x + s);
	int j = fastfloor (y + s);
	int k = fastfloor (z + s);

	float unskew_f = 1.0f / 6.0f; 	// Unskew faktor
	float t = (i + j + k) * unskew_f;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;
	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;

	int i1, j1, k1;
	int i2, j2, k2;

	if (x0 >= y0) {
		if (y0 >= z0) { // X Y Z order
			i1=1; j1=0; k1=0; i2=1; j2=1; k2=0;
		} else if (x0 >= z0) {  // X Z Y order
			i1=1; j1=0; k1=0; i2=1; j2=0; k2=1;
		} else {  // Z X Y order
			i1=0; j1=0; k1=1; i2=1; j2=0; k2=1;
		}
	} else { // x0 < y0
		if (y0 < z0) {  // Z Y X order
			i1=0; j1=0; k1=1; i2=0; j2=1; k2=1;
		} else if (x0<z0) {  // Y Z X order
			i1=0; j1=1; k1=0; i2=0; j2=1; k2=1;
		} else {  // Y X Z order
			i1=0; j1=1; k1=0; i2=1; j2=1; k2=0;
		}
	}

	// c = 1/6.
	float x1 = x0 - i1 + unskew_f;
	float y1 = y0 - j1 + unskew_f;
	float z1 = z0 - k1 + unskew_f;
	float x2 = x0 - i2 + 2.0 * unskew_f;
	float y2 = y0 - j2 + 2.0 * unskew_f;
	float z2 = z0 - k2 + 2.0 * unskew_f;
	float x3 = x0 - 1.0 + 3.0 * unskew_f;
	float y3 = y0 - 1.0 + 3.0 * unskew_f;
	float z3 = z0 - 1.0 + 3.0 * unskew_f;

	/* zjistim index gradientu ctyr vrcholu pomoci hash tabulky */
	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	
	int gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
	int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
	int gi3 = perm[ii + 1 + perm[jj +1 + perm[kk + 1]]] % 12;

	/* Vypocteme prispevek vsech ctyr vrcholu */
	float t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
	
	if (t0 < 0)
		n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0, z0);
	}

	float t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
	
	if (t1 < 0)
		n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1, z1);
	}

	float t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
	
	if (t2 < 0)
		n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2, z2);
	}

	float t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
	
	if (t3 < 0)
		n3 = 0.0;
	else {
		t3 *= t3;
		n3 = t3 * t3 * dot(grad3[gi3], x3, y3, z3);
	}

	/* Secteme vsechny prirustky pro ziskani hodnoty sumu */
	return 32.0 * (n0 + n1 + n2 + n3);	// vysledna hodnota je v rozsahu [-1, 1]
}

terrain_t *terrain_get ()
{
	return terrain_list.next;
}

/* ziskej voxel terenu *t na danych souradnicich */
voxel_t *terrain_voxel_get (terrain_t *t, unsigned x, unsigned y, unsigned z)
{
	if (x >= (t->dim_x+2) || y >= (t->dim_y+2) || z >= (t->dim_z+2))
		return &voxel_null;

	return &t->data[(x+(y*(t->dim_x+2)))+(z*(t->dim_x+2)*(t->dim_y+2))];
}

/* generovani terenu na zaklade jeho pozice */
static bool terrain_gen (terrain_t *t)
{
	if (t->origin_z == 0) {	/* spodni cast terenu */
		for (int x = 0; x < t->dim_x+2; x ++) {
			for (int y = 0; y < t->dim_y+2; y ++) {
				for (int z = 0; z < t->dim_z+2; z ++) {
					voxel_t *v = terrain_voxel_get (t, x, y, z);

					v->value = terrain_noise_get ((float) (x+t->origin_x) / t->dim_x * TERRAIN_GEN_FACTOR,
								(float) (y+t->origin_y) / t->dim_y * TERRAIN_GEN_FACTOR,
								(float) (z+t->origin_z) / t->dim_z * TERRAIN_GEN_FACTOR);
				}
			}
		}
	} else {		/* horni cast terenu */
		for (int x = 0; x < t->dim_x+2; x ++) {
			for (int y = 0; y < t->dim_y+2; y ++) {
				float h = terrain_noise_get ((float) (x+t->origin_x) / t->dim_x * TERRAIN_GEN_FACTOR,
							(float) (y+t->origin_y) / t->dim_y * TERRAIN_GEN_FACTOR,
							(float) (0+t->origin_z) / t->dim_z * TERRAIN_GEN_FACTOR);
				
				for (int z = 0; z < t->dim_z+2; z ++) {				
					voxel_t *v = terrain_voxel_get (t, x, y, z);
					
					/* urcuje kopcovitost terenu, nesmime prekrocit TERRAIN_DIM na vysku */
					if (z < h*30.0f)
						v->value = 1;
					else
						v->value = 0;
				}
				

			}
		}
	}
	
	/* DEPRECATED - testovani voxelu */
	
	/*t->dim_x = 8;
	t->dim_y = 8;
	t->dim_z = 8;

	t->data = (voxel_t *) malloc (sizeof (voxel_t) * t->dim_x * t->dim_y * t->dim_z);*/

	/*for (unsigned x = 0; x < t->dim_x; x ++) {
		for (unsigned y = 0; y < t->dim_y; y ++) {
			for (unsigned z = 0; z < t->dim_z; z ++) {
				voxel_t *v = terrain_voxel_get (t, x, y, z);

				if ((x == 0 || x == 2 || y == 0 || y == 2) && (z == 0 || z == 2))
					v->value = -1;
				else
					v->value = 1 / (0.5f+x+y+z);
			}
		}
	}*/

	/*terrain_voxel_get (t, 0, 1, 2)->value = 1;
	terrain_voxel_get (t, 1, 0, 2)->value = 1;
	terrain_voxel_get (t, 1, 1, 2)->value = 1;
	terrain_voxel_get (t, 1, 2, 2)->value = 1;
	terrain_voxel_get (t, 2, 1, 2)->value = 1;
	
	terrain_voxel_get (t, 1, 1, 3)->value = 1;
	terrain_voxel_get (t, 1, 1, 1)->value = 1;*/
	
/*	terrain_voxel_get (t, 1, 1, 0)->value = 1;
	terrain_voxel_get (t, 0, 1, 0)->value = 1;
	terrain_voxel_get (t, 1, 0, 0)->value = 1;
	terrain_voxel_get (t, 0, 0, 0)->value = 1;*/
	
	/*terrain_voxel_get (t, 0, 0, 1)->value = 1;
	terrain_voxel_get (t, 1, 0, 1)->value = 1;
	terrain_voxel_get (t, 0, 1, 1)->value = 1;
	terrain_voxel_get (t, 1, 1, 1)->value = 1;
	
	terrain_voxel_get (t, 0, 0, 2)->value = 1;
	terrain_voxel_get (t, 1, 0, 2)->value = 1;
	terrain_voxel_get (t, 0, 1, 2)->value = 1;
	terrain_voxel_get (t, 1, 1, 2)->value = 1;
	
	terrain_voxel_get (t, 0, 0, 3)->value = 1;
	terrain_voxel_get (t, 1, 0, 3)->value = 1;
	terrain_voxel_get (t, 0, 1, 3)->value = 1;
	terrain_voxel_get (t, 1, 1, 3)->value = 1;*/
	
	return true;
}

/* modifikace terenu na zaklade akci kamery */
void terrain_regen (camera_t *c, char action)
{
	int d = TERRAIN_DIM;
	
	int dd = d/2;
	
	int v_size = 2;
	
	/* potrebujeme si pamatovat max 4 ruzne kvadranty */
	terrain_t *t_u[4] = { NULL, NULL, NULL, NULL };

	/* chceme vysekat v terenu trojrozmerny utvar */
	for (int i_x = -v_size; i_x < v_size; i_x ++)
	for (int i_y = -v_size; i_y < v_size; i_y ++)
	for (int i_z = -v_size; i_z < v_size; i_z ++) {
		if ((i_x == -v_size || i_x == v_size-1) && (i_y == -v_size || i_y == v_size-1) && (i_z == -v_size || i_z == v_size-1))
			continue;
		
		float v_pos_x = c->pos_x+d/2 + i_x;
		float v_pos_y = c->pos_y+d/2 + i_y;
		float v_pos_z = c->pos_z + i_z;
	
		int cx = - ((int) v_pos_x / d) * d;
		int cy = - ((int) v_pos_y / d) * d;
		int cz = - ((int) v_pos_z / d) * d;
							
		if (c->pos_x < 0)
			cx = - ((int) (c->pos_x-d/2) / d) * d;
		if (c->pos_y < 0)
			cy = - ((int) (c->pos_y-d/2) / d) * d;	

		
		int off_x = 0;
		int off_y = 0;	
again:
		terrain_t *t;
		for (t = terrain_list.next; t != &terrain_list; t = t->next) {						
			if (t->origin_x == cx && t->origin_y == cy && t->origin_z == cz) {
				//t->gl_buf_len = 0;
								
				//printf ("- %d .. %d .. %d\n", cx, cy, cz);
				int v_x = (int) v_pos_x % d;
				int v_y = (int) v_pos_y % d;
				int v_z = (int) v_pos_z % d;
								

				if (v_x >= 0)
					v_x = -(d - v_x);
				if (v_y >= 0)
					v_y = -(d - v_y);
				if (v_z >= 0)
					v_z = -(d - v_z);
				
				v_x += off_x;
				v_y += off_y;
				
			
				/* je nutne aktualizovat teren i za max/min vzdalenosti kvuli zachovani spojitosti */
				
				bool v_c = false;
				voxel_t *v = terrain_voxel_get (t, (unsigned) (-v_x-1), (unsigned) (-v_y-1), (unsigned) -v_z);		
					
					if (action == -1)
						if (v->value >= 0) {
							v->value = -1;
							v_c = true;
						}
					if (action == 1)
						if (v->value < 0) {
							v->value = 1;
							v_c = true;
						}
				
					//printf ("v: %d %d %d\n", v_x, v_y, v_z);			

					if (v_c)
						for (int i = 0; i < 4; i ++)
							if (t_u[i]) {
								if (t_u[i] == t)
									break;
								else
									continue;
							} else
								t_u[i] = t;
				
				/* bohuzel trochu hackovani */
				bool u = false;
				if (v_x == -1) {
					cx -= d;
					off_x -= d;
					u = true;
				}
				if (v_y == -1) {
					cy -= d;
					off_y -= d;
					u = true;
				}
				
				if (v_x == -32) {
					//cx += d;
					off_x --;
					u = true;
				}
				if (v_y == -32) {
					//cy += d;
					off_y --;
					u = true;
				}
				
				if (u)
					goto again;

				break;
			}	
		
		}
	
	}
	
	for (int i = 0; i < 4; i ++)
		if (t_u[i])
			gl_prepare_terrain (t_u[i]);
}

bool terrain_del (terrain_t *t)
{
	//printf ("t_del\n");
	
	t->next->prev = t->prev;
	t->prev->next = t->next;

	void *d = t->data;
	void *bv = t->gl_buf_vert;
	void *bc = t->gl_buf_col;
	void *bt = t->gl_buf_tex;
	void *bt2 = t->gl_buf_tex2;
	
	t->gl_buf_len = 0;
	
	t->root = NULL;
	t->gl_buf_vert = NULL;
	t->gl_buf_col = NULL;
	t->gl_buf_tex = NULL;
	t->gl_buf_tex2 = NULL;
	
	free (d);
	
	free (bv);
	free (bc);
	free (bt);
	//free (bt2);
	
	return true;
}

terrain_t *terrain_add (int x, int y, int z)
{
	terrain_t *t;
	
	/* kontrola, zda neexistuje teren na stejne pozici */
	for (t = terrain_list.next; t != &terrain_list; t = t->next) {
		if (t->origin_x == x && t->origin_y == y && t->origin_z == z) {
			//printf ("teren: %d %d %d existuje!\n", x, y, z);
			return 0;
		}
	}
	
	t = (terrain_t *) calloc (sizeof (terrain_t), 1);

	if (!t)
		return NULL;

	t->origin_x = x;
	t->origin_y = y;
	t->origin_z = z;
	
	t->dim_x = t->dim_y = t->dim_z = TERRAIN_DIM;

	t->data = (voxel_t *) malloc (sizeof (voxel_t) * (t->dim_x+2) * (t->dim_y+2) * (t->dim_z+2));
		
	if (!t->data) {
		free (t);
		return NULL;
	}
	
	/* pridame teren do seznamu jeste pred generovanim, abychom mohli generovat za behu */
	t->next = &terrain_list;
	t->prev = terrain_list.prev;
	t->prev->next = t;
	t->next->prev = t;
	
	if (!terrain_gen (t)) {
		terrain_del (t);
		return NULL;
	}

	/* pripravime teren k vykresleni - polygonizujeme voxely */
	gl_prepare_terrain (t);
	
	//free (t->data);
	//t->data = NULL;
	
	return t;
}

bool terrain_deinit ()
{	
	terrain_t *t;
	for (t = terrain_list.next; t != &terrain_list; t = t->next) {
		terrain_del (t);
	}
}

bool terrain_init ()
{
	terrain_list.next = &terrain_list;
	terrain_list.prev = &terrain_list;
	
	/* pridam prvopocatecni teren */
	if (!terrain_add (0, 0, 0))
		return false;

	return true;
}
