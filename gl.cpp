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
#include "tex.h"
#include "font.h"
#include "water.h"
#include "octree.h"
#include "camera.h"
#include "frustum.h"
#include "terrain.h"
#include "polygonise.h"

static float fps_stick, fps_dtick;
SDL_mutex *gl_mutex;

GLuint filter;                      			// Which Filter To Use
GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };   	// Storage For Three Types Of Fog
GLuint fogfilter = 0;                   			// Which Fog To Use
GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};      	// Fog Color

char prep = 0;

#define RENDER_TERRAIN	1

bool gl_init ()
{
	glClearColor (0.5, 0.5, 1.0, 0.0);
	glClearDepth (1.0);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_TEXTURE_2D);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);
	glDisable (GL_ALPHA_TEST);
	
	glFogi (GL_FOG_MODE, fogMode[1]);	// Fog Mode
	glFogfv (GL_FOG_COLOR, fogColor);            	// Set Fog Color
	glFogf (GL_FOG_DENSITY, 0.008f);              	// How Dense Will The Fog Be
	glHint (GL_FOG_HINT, GL_DONT_CARE);          	// Fog Hint Value
	glFogf (GL_FOG_START, 5.0f);             	// Fog Start Depth
	glFogf (GL_FOG_END, 10.0f);               	// Fog End Depth
	glEnable (GL_FOG);                   		// Enables GL_FOG
	
	
	GLfloat mat_ambient[] = { 0.5, 1.0, 0.5, 1.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_shininess[] = { 120.0 };
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	

	
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	
	
	//glEnable (GL_CULL_FACE);
	//glCullFace (GL_FRONT);
	
	/*glEnable (GL_LIGHTING);

	//glColorMaterial ( GL_FRONT_AND_BACK, GL_EMISSION ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;
	
	GLfloat specular[] = {0.8, 0.8, 0.8, 1.0};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	
	GLfloat ambient[] = {0.8, 0.8, 0.8};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	
	//GLfloat position[] = { -1.5f, 1.0f, -4.0f, 1.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	
	glEnable(GL_LIGHT0);*/
	
	glBlendColor(0.5f, 0.5f, 0.5f, 0.5f);

	gl_mutex = SDL_CreateMutex ();
	if (!gl_mutex) {
		fprintf (stderr, "Couldn't create mutex\n");
		return false;
	}
	
	return true;
}

void gl_resize (int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport (0, 0, width, height);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (60.0, (GLdouble) width / (GLdouble) height, NEAR_PLANE, FAR_PLANE);

	glMatrixMode (GL_MODELVIEW);
}

float gl_fps_get ()
{
	if (!fps_dtick)
		return 1000.0f;

	return 1000.0f / fps_dtick;
}

bool gl_prepare_terrain (terrain_t *t)
{	
	/*
	GLuint list = glGenLists (1);
	
	glNewList (list, GL_COMPILE);
	*/
	
	unsigned trg_num = 0;
	
	float *trg_buf = NULL;
	float *col_buf = NULL;
	float *tex_buf = NULL;
	float *tex2_buf = NULL;
	
	for (int x = 0; x < t->dim_x; x ++)
	for (int y = 0; y < t->dim_y; y ++)
	for (int z = 0; z < t->dim_z; z ++) {
		gridcell_t grid;
		triangle_t trg[5];

		for (int i = 0; i < 8; i ++) {
			int x_i = (((i+1)/2) % 2);
			int y_i = ((i/2) % 2);
			int z_i = (((i)/4) % 2);
			
			grid.p[i].x = x + (double) x_i;
			grid.p[i].y = y + (double) y_i;
			grid.p[i].z = z + (double) z_i;
			
			voxel_t *v_c = terrain_voxel_get (t, x+x_i, z+z_i, y+y_i);
			
			grid.val[i] = v_c->value > 0.0f ? -1 : 0;
		}

		int n = polygonise_grid (grid, 0.0f, &trg[0]);
	
		if (!n)
			continue;

		trg_buf = (float *) realloc ((float *) trg_buf, (trg_num+n) * sizeof (float) * 9);
			
		if (trg_buf == NULL)
			return false;
		
		col_buf = (float *) realloc ((float *) col_buf, (trg_num+n) * sizeof (float) * 9);
			
		if (col_buf == NULL)
			return false;
		
		tex_buf = (float *) realloc ((float *) tex_buf, (trg_num+n) * sizeof (float) * 6);
			
		if (tex_buf == NULL)
			return false;

		/*tex2_buf = (float *) realloc ((float *) tex2_buf, (trg_num+n) * sizeof (float) * 6);
			
		if (tex2_buf == NULL)
			return false;*/
		
		/* pocet trojuhelniku k vykresleni */
		for (int i = 0; i < n; i ++) {
			vector_t n = polygonise_trg_norm (trg[i]);
			
			float h = y*5;
			if (t->origin_z == 0)
				h = 0;
			
			float color = 0;
			
			/*bool snow = false;
			
			if (y > 24 && h) {
				(rand () % 20) != 0 ? snow = false : snow = true;
			}*/
			
			color = (190-h) / 255.0f;
			
			//if (snow)
			//	color = 255.0f;
			
			int idx = (trg_num+i) * 9;
			
			trg_buf[idx+0] = trg[i].p[0].x;
			trg_buf[idx+1] = trg[i].p[0].y;
			trg_buf[idx+2] = trg[i].p[0].z;
			
			trg_buf[idx+3] = trg[i].p[1].x;
			trg_buf[idx+4] = trg[i].p[1].y;
			trg_buf[idx+5] = trg[i].p[1].z;
			
			trg_buf[idx+6] = trg[i].p[2].x;
			trg_buf[idx+7] = trg[i].p[2].y;
			trg_buf[idx+8] = trg[i].p[2].z;
			
			col_buf[idx+0] = 0;
			col_buf[idx+1] = color;
			col_buf[idx+2] = 0;
			
			col_buf[idx+3] = 0;
			col_buf[idx+4] = color;
			col_buf[idx+5] = 0;
			
			col_buf[idx+6] = 0;
			col_buf[idx+7] = color;
			col_buf[idx+8] = 0;
			
			int tidx = (trg_num+i) * 6;
			
			tex_buf[tidx+0] = 0;
			tex_buf[tidx+1] = 0;
				
			tex_buf[tidx+2] = 1;
			tex_buf[tidx+3] = 0;
				
			tex_buf[tidx+4] = 1;
			tex_buf[tidx+5] = 1;
		}
		
		trg_num	+= n;
	}

	void *buf_vert = t->gl_buf_vert;
	void *buf_col = t->gl_buf_col;
	void *buf_tex = t->gl_buf_tex;
	
	t->gl_buf_len = trg_num*3;
	
	t->gl_buf_vert = trg_buf;
	t->gl_buf_col = col_buf;
	t->gl_buf_tex = tex_buf;

	if (t->gl_buf_len) {
		free (buf_vert);
		free (buf_col);
		free (buf_tex);
	}
	
	prep = 1;
	
	//t->gl_buf_tex2 = tex2_buf;
	//glEndList ();
	
	return true;
}

void gl_delete_terrain (terrain_t *t)
{
	//glDeleteLists (t->gl_list, 1);
}

/* vykreslovani SkyBoxu - enviromentalni mapy */
void gl_render_skybox ()
{
	glPushMatrix ();

	glScalef (350, 350, 350);
	
	/* vykreslime predni stranu krychle */
	glBindTexture (GL_TEXTURE_2D, tex_get (1));
	glBegin (GL_QUADS);
		glTexCoord2f (0, 0); glVertex3f (  0.5f, -0.5f, -0.5f );
		glTexCoord2f (1, 0); glVertex3f ( -0.5f, -0.5f, -0.5f );
		glTexCoord2f (1, 1); glVertex3f ( -0.5f,  0.5f, -0.5f );
		glTexCoord2f (0, 1); glVertex3f (  0.5f,  0.5f, -0.5f );
	glEnd();
	
	/* vykreslime levou stranu krychle */
	glBindTexture (GL_TEXTURE_2D, tex_get (2));
	glBegin (GL_QUADS);
		glTexCoord2f (0, 0); glVertex3f (  0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 0); glVertex3f (  0.5f, -0.5f, -0.5f );
		glTexCoord2f (1, 1); glVertex3f (  0.5f,  0.5f, -0.5f );
		glTexCoord2f (0, 1); glVertex3f (  0.5f,  0.5f,  0.5f );
	glEnd ();
	
	/* vykreslime zadni stranu krychle */
	glBindTexture (GL_TEXTURE_2D, tex_get (3));
	glBegin (GL_QUADS);
		glTexCoord2f (0, 0); glVertex3f ( -0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 0); glVertex3f (  0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 1); glVertex3f (  0.5f,  0.5f,  0.5f );
		glTexCoord2f (0, 1); glVertex3f ( -0.5f,  0.5f,  0.5f );
	
	glEnd ();
	
	/* vykreslime pravou stranu krychle */
	glBindTexture (GL_TEXTURE_2D, tex_get (4));
	glBegin (GL_QUADS);
		glTexCoord2f (0, 0); glVertex3f ( -0.5f, -0.5f, -0.5f );
		glTexCoord2f (1, 0); glVertex3f ( -0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 1); glVertex3f ( -0.5f,  0.5f,  0.5f );
		glTexCoord2f (0, 1); glVertex3f ( -0.5f,  0.5f, -0.5f );
	glEnd ();
	
	/* vykreslime vrchni stranu krychle */
	glBindTexture (GL_TEXTURE_2D, tex_get (5));
	glBegin (GL_QUADS);
		glTexCoord2f (0, 1); glVertex3f ( -0.5f,  0.5f, -0.5f );
		glTexCoord2f (0, 0); glVertex3f ( -0.5f,  0.5f,  0.5f );
		glTexCoord2f (1, 0); glVertex3f (  0.5f,  0.5f,  0.5f );
		glTexCoord2f (1, 1); glVertex3f (  0.5f,  0.5f, -0.5f );
	glEnd ();
	
	/* vykreslime spodni stranu krychle */
	glBindTexture(GL_TEXTURE_2D, tex_get (6));
	glBegin(GL_QUADS);
		glTexCoord2f (0, 0); glVertex3f ( -0.5f, -0.5f, -0.5f );
		glTexCoord2f (0, 1); glVertex3f ( -0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 1); glVertex3f (  0.5f, -0.5f,  0.5f );
		glTexCoord2f (1, 0); glVertex3f (  0.5f, -0.5f, -0.5f );
	glEnd();
	
	glPopMatrix ();	
}

/* DEPRECATED - pravdepodobne se jiz nebude vyuzivat */
void gl_render_octree_mesh (terrain_t *t, camera_t *c, octree_t *node)
{
	if (!node)
		return;
	
	if (node->children)
	for (int i = 0; i < 8; i ++) {
		if (node->children[i])
			gl_render_octree_mesh (t, c, node->children[i]);	
	}
	
	if (node->size_x != c->view_dist)
		return;
	
	glPushMatrix ();
	glTranslatef ((t->origin_x+node->origin_x)*TERRAIN_VOXEL_DIM-1,
		      (t->origin_z+node->origin_z)*TERRAIN_VOXEL_DIM-1,
		      (t->origin_y+node->origin_y)*TERRAIN_VOXEL_DIM-1);
			
	glScalef (node->size_x*1.001f, node->size_z*1.001f, node->size_y*1.001f);
		
	float c_q = 255;

	glBegin (GL_QUADS);
		// Front Face
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1.0f,  1.0f);  
		// Back Face
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f, -1.0f);  
		// Top Face
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1, -1.0f);  
		// Bottom Face
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f,  1.0f);  
		// Right face
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f,  1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f ( 1.0f, -1.0f,  1.0f);  
		// Left Face
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f, -1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f, -1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1.0f,  1.0f);  
		glColor4ub (0, c_q, 0, 60); glVertex3f (-1.0f,  1.0f, -1.0f);  
	glEnd ();
	
	glPopMatrix ();
}

void gl_render_terrain (terrain_t *t, camera_t *c)
{
// 	extern int polygonise_grid (gridcell_t grid, double isolevel, triangle_t *triangles);

	if (!t->gl_buf_len)
		return;
#ifdef FRUSTUM
	if (frustum_check (t, c) == false)
		return;
#endif
	
	glPushMatrix ();

	glTranslatef (t->origin_x-TERRAIN_DIM, t->origin_z, t->origin_y-TERRAIN_DIM);
	
	glBindTexture (GL_TEXTURE_2D, tex_get (0));
	
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_COLOR_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		
	glColorPointer (3, GL_FLOAT, 0, t->gl_buf_col);
	glVertexPointer (3, GL_FLOAT, 0, t->gl_buf_vert);
	glTexCoordPointer (2, GL_FLOAT, 0, t->gl_buf_tex);
	
	/*glClientActiveTexture (GL_TEXTURE0);
	glActiveTexture (GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
	glBindTexture (GL_TEXTURE_2D, tex_get (0));
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
	glTexCoordPointer (2, GL_FLOAT, 0, t->gl_buf_tex);
	glEnable(GL_TEXTURE_2D);
	
	glClientActiveTexture (GL_TEXTURE1);
 	glActiveTexture (GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture (GL_TEXTURE_2D, tex_get (1));
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glTexCoordPointer (2, GL_FLOAT, 0, t->gl_buf_tex2);
	glEnable(GL_TEXTURE_2D);*/
	
	/* k vykresleni pouzivame Vertex Arrays */
	glDrawArrays (GL_TRIANGLES, 0, t->gl_buf_len);
	
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	
	glPopMatrix ();
}

void gl_render ()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
	
	/* Vykreslovani HUD/textu */
	char sfps[32];
	sprintf (sfps, "FPS: %.0f", 1.0f/(fps_dtick/1000.0f));
	font_render (-350, -250, sfps);
	glLoadIdentity ();
	
	/* Ziskame nasi kameru */
	camera_t *c = camera_get ();

	/* scene motion */
	glRotatef (c->rot_y, 1, 0, 0);
	glRotatef (c->rot_x, 0, 1, 0);
	
	/* Vykreslovani SkyBoxu */
	gl_render_skybox ();
	
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	/* Vykreslovani terenu */
	glTranslatef (c->pos_x+TERRAIN_DIM/2, c->pos_z, c->pos_y+TERRAIN_DIM/2);
	
	terrain_t *t;
	
	for (t = terrain_list.next; t != &terrain_list; t = t->next) {
		gl_render_terrain (t, c);
		//gl_render_terrain (terrain_list.next, c);
			
		//gl_render_octree_mesh (t, c, (octree_t *) t->root);
	}
	
	/* Vykreslovani vody */
	glTranslatef (-c->pos_x-TERRAIN_DIM/2, 0, -c->pos_y-TERRAIN_DIM/2);
	
	gl_water_render ();

	glFlush ();
	
	 /* Prohodi predni a zadni buffer */
	SDL_GL_SwapWindow (g_window);

	/* vypocitame hodnotu FPS na zaklade soucasneho casu a casu z predchoziho cyklu */
	fps_dtick = SDL_GetTicks () - fps_stick;

	/* Omezime shora FPS, protoze nam vyssi uz nic neprinese */
	if (1000/FPS_MAX > fps_dtick)
		SDL_Delay (1000.0f/FPS_MAX - fps_dtick);

	fps_stick = SDL_GetTicks ();
}

