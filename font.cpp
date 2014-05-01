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

#include "font.h"
#include "vbo.h"
#include "shader.h"
#include "camera.h"

#ifndef ANDROID
static TTF_Font *font;
static int vbo_id;
static int shader;
#endif
void font_render (const double &X, const double &Y, const char *text)
{
#ifndef ANDROID
	unsigned Texture = 0;

	/* barva pisma.*/
	SDL_Color color = {255, 255, 255};
	
	SDL_Surface *msg = TTF_RenderText_Blended (const_cast<TTF_Font*>(font), text, color);
 
	/* vygenerujeme texturu v OpenGL z SDL_Surface.*/

	glGenTextures (1, &Texture);
	glBindTexture (GL_TEXTURE_2D, Texture);
 
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, msg->w, msg->h, 0, GL_BGRA,
	             GL_UNSIGNED_BYTE, msg->pixels);

	camera_t *c = camera_get ();
	
	glBindBuffer (GL_ARRAY_BUFFER, vbo_id);
	
	glUseProgram (shader);
		
	glm::mat4 proj = c->projection;
	glm::mat4 tmp = glm::translate (glm::vec3 (X, Y, -1.0f)) * glm::rotate (180.0f, glm::vec3 (1, 0, 0)) * glm::scale (glm::vec3 (0.1f, 0.05f, 1.0));

	int uniform = glGetUniformLocation (shader, "MVMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&tmp[0]);
	uniform = glGetUniformLocation (shader, "PMatrix");
	glUniformMatrix4fv (uniform, 1, GL_FALSE, (float*)&proj[0]);
	
	GLuint tex_id = glGetUniformLocation (shader, "tex_sampler0");
	glUniform1i (tex_id, 0);

	/* texura */
	glActiveTexture (GL_TEXTURE0); 

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, (5 * sizeof(GLfloat)), 0);
	
	/* k vykresleni pouzivame Vertex Buffer Object */
	glDrawArrays (GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray (0);
	glDisableVertexAttribArray (1);

	glBindBuffer (GL_ARRAY_BUFFER, 0);

	/* vypneme shader */
	glUseProgram (0);

	/*Clean up.*/
	glDeleteTextures(1, &Texture);
	SDL_FreeSurface(msg);
#endif
} 

bool font_init ()
{
#ifndef ANDROID
	TTF_Init ();
	
	font = TTF_OpenFont ("data/DejaVuSans.ttf", 12);

	if (!font)
		return false;
#endif
	GLfloat buf[] = { 
		//x, y, z, u ,v
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};
	
	vbo_id = vbo_alloc ((void *) buf, sizeof (buf));
	shader = shader_init ("data/shader_font");
	
	return true;
}