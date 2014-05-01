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
 *   Based on the examples from http://paulbourke.net/geometry/polygonise/ @ Public Domain
 */

#include "gen.h"
#include "polygonise.h"
#include "polytable.h"

/* Normalizace vektoru */
static void trg_normalize (vector_t *v)
{
	double l = sqrt ((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
	
	if (l == 0.0)
		return;
	
	float s = 1.0 / l;
		
	v->x *= s;
	v->y *= s;
	v->z *= s;
}

/* Normalizace trojuhelniku */
vector_t polygonise_trg_norm (triangle_t t)
{
	vector_t u, v, n;
	
	u.x = t.p[1].x - t.p[0].x;
	u.y = t.p[1].y - t.p[0].y;
	u.z = t.p[1].z - t.p[0].z;
	
	v.x = t.p[2].x - t.p[0].x;
	v.y = t.p[2].y - t.p[0].y;
	v.z = t.p[2].z - t.p[0].z;
	
	n.x = -((u.y * v.z) - (u.z * v.y));
	n.y = -((u.z * v.x) - (u.x * v.z));
	n.z = -((u.x * v.y) - (u.y * v.x));
	
	trg_normalize (&n);
	
	return n;
}

/* Linearni interpolace pozice kde isosurface protina hranu mezi dvemi vrcholy,
 * kazdy se svym skalarem */

static vertex_t vertex_interp (double isolevel, vertex_t p1, vertex_t p2, double valp1, double valp2)
{
	double mu;
	vertex_t p;

	if (abs (isolevel - valp1) < POLY_EPSILON)
		return p1;
	if (abs (isolevel - valp2) < POLY_EPSILON)
		return p2;
	if (abs (valp1 - valp2) < POLY_EPSILON)
		return p1;
	
	mu = (isolevel - valp1) / (valp2 - valp1);
	
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return p;
}

int polygonise_grid (gridcell_t grid, double isolevel, triangle_t *triangles)
{
	int i, trg_num, cube_idx;
	vertex_t vertlist[12];
   
	/* Nejprve zjistime index pro urceni vrcholu, ktere tvori povrch.
	   Tento index nam udava umisteni dat v table_edge */
	
	cube_idx = 0;
	if (grid.val[0] < isolevel)
		cube_idx |= 1;
	if (grid.val[1] < isolevel)
		cube_idx |= 2;
	if (grid.val[2] < isolevel)
		cube_idx |= 4;
	if (grid.val[3] < isolevel)
		cube_idx |= 8;
	if (grid.val[4] < isolevel)
		cube_idx |= 16;
	if (grid.val[5] < isolevel)
		cube_idx |= 32;
	if (grid.val[6] < isolevel)
		cube_idx |= 64;
	if (grid.val[7] < isolevel)
		cube_idx |= 128;

	/* Krychle je mimo sledovany povrch */
	if (!table_edge[cube_idx])
		return 0;

	/* Zjistime jednotlive vrcholy kde se povrch proti s krychli */
	if (table_edge[cube_idx] & 1)
		vertlist[0] = vertex_interp (isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
	if (table_edge[cube_idx] & 2)
		vertlist[1] = vertex_interp (isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
	if (table_edge[cube_idx] & 4)
		vertlist[2] = vertex_interp (isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
	if (table_edge[cube_idx] & 8)
		vertlist[3] = vertex_interp (isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
	if (table_edge[cube_idx] & 16)
		vertlist[4] = vertex_interp (isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
	if (table_edge[cube_idx] & 32)
		vertlist[5] = vertex_interp (isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
	if (table_edge[cube_idx] & 64)
		vertlist[6] = vertex_interp (isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
	if (table_edge[cube_idx] & 128)
		vertlist[7] = vertex_interp (isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
	if (table_edge[cube_idx] & 256)
		vertlist[8] = vertex_interp (isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
	if (table_edge[cube_idx] & 512)
		vertlist[9] = vertex_interp (isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
	if (table_edge[cube_idx] & 1024)
		vertlist[10] = vertex_interp (isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
	if (table_edge[cube_idx] & 2048)
		vertlist[11] = vertex_interp (isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

	/* vytvorime pole trojuhelniku, ktere se maji vykreslit */
	trg_num = 0;
	
	for (i = 0; table_trg[cube_idx][i] != -1; i += 3) {
		triangles[trg_num].p[0] = vertlist[table_trg[cube_idx][i]];
		triangles[trg_num].p[1] = vertlist[table_trg[cube_idx][i+1]];
		triangles[trg_num].p[2] = vertlist[table_trg[cube_idx][i+2]];
		
		trg_num ++;
	}

	return trg_num;	// vrati pocet pouzitelnych trojuhelniku v poli (max. 5)
}

bool polygonise_init ()
{	
	return true;
}
