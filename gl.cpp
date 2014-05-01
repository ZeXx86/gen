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
#include "vbo.h"
#include "gen.h"
#include "tex.h"
#include "font.h"
#include "water.h"
#include "shader.h"
#include "octree.h"
#include "camera.h"
#include "frustum.h"
#include "terrain.h"
#include "polygonise.h"

static float fps_stick, fps_dtick;
SDL_mutex *gl_mutex;

static GLuint shader[2];

static light_t light[1];
static material_t mat[1];

void gl_init_skybox ();
static unsigned vbo_skybox_id;

#define RENDER_TERRAIN	1

bool gl_init ()
{
	glClearColor (0.5, 0.5, 1.0, 0.0);
#ifdef ANDROID
	glClearDepthf (1.0);
#else
	glClearDepth (1.0);
#endif
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_TEXTURE_2D);
#ifndef ANDROID
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);
	glDisable (GL_ALPHA_TEST);
#endif
	/* nacteni a kompilace shaderu */
	shader[0] = shader_init ("data/shader_ter");
	shader[1] = shader_init ("data/shader_skybox");
	
	/* definice osvetleni */
	light[0].ambient[0] = light[0].ambient[1] = light[0].ambient[2] = 0.2f;
	light[0].ambient[3] = 1.0f;
	light[0].diffuse[0] = light[0].diffuse[1] = light[0].diffuse[2] = 0.3f;
	light[0].diffuse[3] = 1.0f;
	light[0].specular[0] = light[0].specular[1] = light[0].specular[2] = 0.6f;
	light[0].specular[3] = 1.0f;
	light[0].position[0] = 0.0f;
	light[0].position[1] = 0.0f;
 	light[0].position[2] = 100.0f;
	
	light[0].position[3] = 0.0f; 	
	light[0].name = strdup ("light");

	/* definice materialu */
	mat[0].ambient[0] = mat[0].ambient[1] = mat[0].ambient[2] = 1.4f;
	mat[0].ambient[3] = 1.0f;
	mat[0].diffuse[0] = mat[0].diffuse[1] = mat[0].diffuse[2] = 0.1f;
	mat[0].diffuse[3] = 1.0f;
	mat[0].specular[0] = mat[0].specular[1] = mat[0].specular[2] = mat[0].specular[3] = 0.15f;
	mat[0].name = strdup ("material");
	
	gl_mutex = SDL_CreateMutex ();
	if (!gl_mutex) {
		fprintf (stderr, "Couldn't create mutex\n");
		return false;
	}
	
	gl_init_skybox ();

	return true;
}

void gl_init_skybox ()
{
	const GLfloat buf[] = { 
		//x, y, z, u ,v
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
	};
	
	vbo_skybox_id = vbo_alloc ((void *) buf, sizeof (buf));
}

void gl_resize (int width, int height)
{
	camera_init (0, 0, width, height);
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
		
		float voxel_val = 0;

		for (int i = 0; i < 8; i ++) {
			int x_i = (((i+1)/2) % 2);
			int y_i = ((i/2) % 2);
			int z_i = (((i)/4) % 2);
			
			grid.p[i].x = x + (double) x_i;
			grid.p[i].y = y + (double) y_i;
			grid.p[i].z = z + (double) z_i;
			
			voxel_t *v_c = terrain_voxel_get (t, x+x_i, z+z_i, y+y_i);
			
			grid.val[i] = v_c->value > 0.0f ? -1 : 0;
			
			voxel_val += v_c->value / 8.0f;
		}

		int n = polygonise_grid (grid, 0.0f, &trg[0]);
	
		if (!n)
			continue;

		trg_buf = (float *) realloc ((float *) trg_buf, (trg_num+n) * sizeof (float) * (9 + 9 + 9));
			
		if (trg_buf == NULL)
			return false;
	
		/* pocet trojuhelniku k vykresleni */
		for (int i = 0; i < n; i ++) {
			vector_t n = polygonise_trg_norm (trg[i]);
			
			float h = y*5;
			if (t->origin_z == 0)
				h = 0;
			
			//float color = 0;
			
			/*bool snow = false;
			
			if (y > 24 && h) {
				(rand () % 20) != 0 ? snow = false : snow = true;
			}*/
			
			//color = (190-h) / 255.0f;
			
			//if (snow)
			//	color = 255.0f;
			
			/*glm::vec3 v1 = glm::vec3 (trg[i].p[0].x, trg[i].p[0].y, trg[i].p[0].z);
			glm::vec3 v2 = glm::vec3 (trg[i].p[1].x, trg[i].p[1].y, trg[i].p[1].z);
			glm::vec3 v3 = glm::vec3 (trg[i].p[2].x, trg[i].p[2].y, trg[i].p[2].z);
			
			normal = glm::normalize (glm::cross (v3 - v1, v2 - v1));*/
			
			int idx = (trg_num+i) * 27;
			
			trg_buf[idx+0] = trg[i].p[0].x;
			trg_buf[idx+1] = trg[i].p[0].y;
			trg_buf[idx+2] = trg[i].p[0].z;
			
			trg_buf[idx+3] = n.x;
			trg_buf[idx+4] = n.y;
			trg_buf[idx+5] = n.z;
			
			trg_buf[idx+6] = 0;
			trg_buf[idx+7] = 0;
			
			trg_buf[idx+8] = h;
			
			idx += 9;
			
			trg_buf[idx+0] = trg[i].p[1].x;
			trg_buf[idx+1] = trg[i].p[1].y;
			trg_buf[idx+2] = trg[i].p[1].z;
			
			trg_buf[idx+3] = n.x;
			trg_buf[idx+4] = n.y;
			trg_buf[idx+5] = n.z;
			
			trg_buf[idx+6] = 1;
			trg_buf[idx+7] = 1;
			
			trg_buf[idx+8] = h;
			
			idx += 9;
			
			trg_buf[idx+0] = trg[i].p[2].x;
			trg_buf[idx+1] = trg[i].p[2].y;
			trg_buf[idx+2] = trg[i].p[2].z;

			trg_buf[idx+3] = n.x;
			trg_buf[idx+4] = n.y;
			trg_buf[idx+5] = n.z;
							
			trg_buf[idx+6] = 1;
			trg_buf[idx+7] = 0;
			
			trg_buf[idx+8] = h;
		}
		
		trg_num	+= n;
	}

	void *gl_buf = t->gl_buf;

	t->gl_buf_len = trg_num*3;
	
	t->gl_buf = trg_buf;
	
	if (t->gl_buf_len) {
		free (gl_buf);
		//free (buf_col);
		//free (buf_tex);
	}

	
	return true;
}

void gl_delete_terrain (terrain_t *t)
{
	//glDeleteLists (t->gl_list, 1);
}

/* vykreslovani SkyBoxu - enviromentalni mapy */
void gl_render_skybox (camera_t *c)
{
	glDisable (GL_DEPTH_TEST);
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo_skybox_id);

	glUseProgram (shader[1]);
		
	glm::mat4 proj = c->projection;
	glm::mat4 tmp = c->view;

	int uniform = glGetUniformLocation (shader[1], "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader[1], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&proj[0]);
	
				
	GLuint tex_id = glGetUniformLocation (shader[1], "tex_sampler0");
	glUniform1i (tex_id, 0);

	/* texura */
	glActiveTexture (GL_TEXTURE0); 
	glBindTexture (GL_TEXTURE_CUBE_MAP, tex_get (0));

	glEnableVertexAttribArray (0);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(GLfloat)), 0);
	
	/* k vykresleni pouzivame Vertex Buffer Object */
	glDrawArrays (GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray (0);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);

	/* vypneme shader */
	glUseProgram (0);
	
	glEnable (GL_DEPTH_TEST);
}

void gl_render_terrain (terrain_t *t, camera_t *c)
{
//#define FRUSTUM 1
#ifdef FRUSTUM
	if (frustum_check (t, c) == false)
		return;
#endif
	if (t->state & TERRAIN_STATE_UPDATE)
		vbo_free (&t->vbo_id);
	
	if (!t->vbo_id)
		t->vbo_id = vbo_alloc (t->gl_buf, t->gl_buf_len * sizeof (float) * 9);
	
	glBindBuffer (GL_ARRAY_BUFFER, t->vbo_id);
	
	/* Vykreslovani terenu */
	glm::mat4 mdl_matrix;
	
	mdl_matrix = glm::translate (glm::vec3 (t->origin_x-TERRAIN_DIM, t->origin_z, t->origin_y-TERRAIN_DIM));

	/* enable program and set uniform variables */
	glUseProgram (shader[0]);
			
	glm::mat4 tmp = c->view * mdl_matrix;

	int uniform = glGetUniformLocation (shader[0], "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&c->projection[0]);
	uniform = glGetUniformLocation (shader[0], "VMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&c->view[0]);
	uniform = glGetUniformLocation (shader[0], "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader[0], "NormalMatrix");
	glUniformMatrix3fv (uniform, 1, GL_FALSE, (float*)&(glm::inverseTranspose(glm::mat3(tmp)))[0]);
			
	shader_getuniform_light (shader[0], &light[0]);
	shader_getuniform_material (shader[0], &mat[0]);
				
	GLuint tex_id  = glGetUniformLocation (shader[0], "tex_sampler0");
	glUniform1i (tex_id, 0);
	tex_id  = glGetUniformLocation (shader[0], "tex_sampler1");
	glUniform1i (tex_id, 1);
	tex_id  = glGetUniformLocation (shader[0], "tex_sampler2");
	glUniform1i (tex_id, 2);

	/* texura */
	glActiveTexture (GL_TEXTURE2); 
	glBindTexture (GL_TEXTURE_2D, tex_get (4));
	
	glActiveTexture (GL_TEXTURE1); 
	glBindTexture (GL_TEXTURE_2D, tex_get (3));
			
	glActiveTexture (GL_TEXTURE0); 
	glBindTexture (GL_TEXTURE_2D, tex_get (2));


	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	glEnableVertexAttribArray (3);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(GLfloat)), (GLvoid *) (3 * sizeof(GLfloat)));
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, (9 * sizeof(GLfloat)), (GLvoid *) (6 * sizeof(GLfloat)));
	glVertexAttribPointer (3, 1, GL_FLOAT, GL_FALSE, (9 * sizeof(GLfloat)), (GLvoid *) (8 * sizeof(GLfloat)));
	
	/* k vykresleni pouzivame Vertex Buffer Object */
	glDrawArrays (GL_TRIANGLES, 0, t->gl_buf_len);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);
	glDisableVertexAttribArray (2);
	glDisableVertexAttribArray (3);
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	/* vypneme shader */
	glUseProgram (0);
}

void gl_render ()
{
	fps_stick = SDL_GetTicks ();
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
	
	camera_update ();
	
	/* Ziskame nasi kameru */
	camera_t *c = camera_get ();
	
	/* Vykreslovani SkyBoxu */
	gl_render_skybox (c);
	
	terrain_t *t;	
	for (t = terrain_list.next; t != &terrain_list; t = t->next) {
		if (!t->gl_buf_len) {
			/* mazani kontextu terenu z pameti */
			terrain_t *tmp = t->prev;
			if (terrain_del (t))
				t = tmp;
			
			continue;
		}

		gl_render_terrain (t, c);	
	}

	/* Vykreslovani vody */
	gl_water_render ();

	/* Vykreslovani HUD/textu */
	char sfps[32];
	sprintf (sfps, "FPS: %.0f", 1.0f/(fps_dtick/1000.0f));
	font_render (0.500, -0.400, sfps);
	
	glFlush ();
	
	 /* Prohodi predni a zadni buffer */
	SDL_GL_SwapWindow (g_window);

	/* vypocitame hodnotu FPS na zaklade soucasneho casu a casu z predchoziho cyklu */
	fps_dtick = SDL_GetTicks () - fps_stick;

	/* Omezime shora FPS, protoze nam vyssi uz nic neprinese */
	if (1000/FPS_MAX > fps_dtick)
		SDL_Delay (1000.0f/FPS_MAX - fps_dtick);
}

