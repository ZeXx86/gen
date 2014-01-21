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
 *   Based on the example from http://www.lousodrome.net/opengl/water.tar.gz @ GNU/GPL2 
 */


#include "gl.h"
#include "gen.h"
#include "tex.h"
#include "water.h"

static float *water_surf;
static float *water_norm;

static float gl_water_z (const float x, const float y, const float t)
{
	const float x2 = x - 3;
	const float y2 = y + 1;
	const float xx = x2 * x2;
	const float yy = y2 * y2;
	
	return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) + terrain_noise_get (10 * x, 10 * y, t)) / 200);
}

void gl_water_render ()
{
	glPushMatrix ();	
	
	unsigned int indice;
	unsigned int preindice;
	
	const unsigned int length = 2 * (WATER_RES + 1);

	const float t = SDL_GetTicks () / 2000.0f;	// "casovani" vln
	const float delta = 2. / WATER_RES;
	const float xn = (WATER_RES + 1) * delta + 1;
	
	float v1[3];
	float v2[3];
	float v3[3];
	float va[3];
	float vb[3];
	float n[3];
	float x, y;
	
	/* vrcholy */
	for (int j = 0; j < WATER_RES; j ++) {
		y = (j + 1) * delta - 1;
		
		for (int i = 0; i <= WATER_RES; i ++) {
			indice = 6 * (i + j * (WATER_RES + 1));

			x = i * delta - 1;
			water_surf[indice + 3] = x * WATER_DIM;
			water_surf[indice + 4] = gl_water_z (x, y, t) * 150;
			water_surf[indice + 5] = y * WATER_DIM;
			
			if (j) {
				/* Hodnoty predpocitany z predchoziho cyklu */
				preindice = 6 * (i + (j - 1) * (WATER_RES + 1));
				water_surf[indice] = water_surf[preindice + 3];
				water_surf[indice + 1] = water_surf[preindice + 4];
				water_surf[indice + 2] = water_surf[preindice + 5];
			} else {
				water_surf[indice] = x * WATER_DIM;
				water_surf[indice + 1] = gl_water_z (x, -1, t) * 150;
				water_surf[indice + 2] = -1 * WATER_DIM;
			}
		}
	}

	/* normaly */
	for (int j = 0; j < WATER_RES; j ++)
	for (int i = 0; i <= WATER_RES; i ++) {
		indice = 6 * (i + j * (WATER_RES + 1));

		v1[0] = water_surf[indice + 3];
		v1[1] = water_surf[indice + 4];
		v1[2] = water_surf[indice + 5];

		v2[0] = v1[0];
		v2[1] = water_surf[indice + 1];
		v2[2] = water_surf[indice + 2];

		if (i < WATER_RES) {
			v3[0] = water_surf[indice + 9];
			v3[1] = water_surf[indice + 10];
			v3[2] = v1[2];
		} else {
			v3[0] = xn;
			v3[1] = gl_water_z (xn, v1[2], t);
			v3[2] = v1[2];
		}

		va[0] = v2[0] - v1[0];
		va[1] = v2[1] - v1[1];
		va[2] = v2[2] - v1[2];

		vb[0] = v3[0] - v1[0];
		vb[1] = v3[1] - v1[1];
		vb[2] = v3[2] - v1[2];

		n[0] = (vb[1] * va[2]) - (vb[2] * va[1]);
		n[1] = (vb[2] * va[0]) - (vb[0] * va[2]);
		n[2] = (vb[0] * va[1]) - (vb[1] * va[0]);

		float l = sqrtf (n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		
		if (l) {
			l = 1 / l;
			water_norm[indice + 3] = n[0] * l;
			water_norm[indice + 4] = n[1] * l;
			water_norm[indice + 5] = n[2] * l;
		} else {
			water_norm[indice + 3] = 0;
			water_norm[indice + 4] = 1;
			water_norm[indice + 5] = 0;
		}

		if (j) {
			/* Hodnoty predpocitany z predchoziho cyklu */
			preindice = 6 * (i + (j - 1) * (WATER_RES + 1));
			water_norm[indice] = water_norm[preindice + 3];
			water_norm[indice + 1] = water_norm[preindice + 4];
			water_norm[indice + 2] = water_norm[preindice + 5];
		} else {
			/* v1[0] = v1[0]; */
			v1[1] = gl_water_z (v1[0], (j - 1) * delta - 1, t);
			v1[2] = (j - 1) * delta - 1;

			/* v3[0] = v3[0]; */
			v3[1] = gl_water_z (v3[0], v2[2], t);
			v3[2] = v2[2];

			va[0] = v1[0] - v2[0];
			va[1] = v1[1] - v2[1];
			va[2] = v1[2] - v2[2];

			vb[0] = v3[0] - v2[0];
			vb[1] = v3[1] - v2[1];
			vb[2] = v3[2] - v2[2];

			n[0] = (vb[1] * va[2]) - (vb[2] * va[1]);
			n[1] = (vb[2] * va[0]) - (vb[0] * va[2]);
			n[2] = (vb[0] * va[1]) - (vb[1] * va[0]);

			float l = sqrtf (n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
			
			if (l) {
				l = 1 / l;
				water_norm[indice] = n[0] * l;
				water_norm[indice + 1] = n[1] * l;
				water_norm[indice + 2] = n[2] * l;
			} else {
				water_norm[indice] = 0;
				water_norm[indice + 1] = 1;
				water_norm[indice + 2] = 0;
			}
		}
	}

	glTranslatef (0, 0.4f, 0);	

	glEnable (GL_TEXTURE_GEN_S);
	glEnable (GL_TEXTURE_GEN_T);
	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glBindTexture(GL_TEXTURE_2D, tex_get (7));
	
	//glEnable (GL_DEPTH_TEST);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_VERTEX_ARRAY);
	glNormalPointer (GL_FLOAT, 0, water_norm);
	glVertexPointer (3, GL_FLOAT, 0, water_surf);

	for (int i = 0; i < WATER_RES; i ++)
		glDrawArrays (GL_TRIANGLE_STRIP, i * length, length);

	//glDisable (GL_TEXTURE_2D);
	
	glDisable (GL_TEXTURE_GEN_S);
	glDisable (GL_TEXTURE_GEN_T);
	
	//glDisable (GL_BLEND);
	
	glPopMatrix ();	
}

bool gl_water_init ()
{
	water_surf = (float *) malloc (6 * WATER_RES * (WATER_RES + 1) * sizeof (float));
	
	if (!water_surf)
		return false;
	
	water_norm = (float *) malloc (6 * WATER_RES * (WATER_RES + 1) * sizeof (float));
	
	if (!water_norm)
		return false;
	
	return true;
}
