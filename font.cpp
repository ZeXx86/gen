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

static TTF_Font *font;

void font_render (const double &X, const double &Y, const char *text)
{
	/* vzdalenost od obrazovky */
	const double Z = 0.5f;
	
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
	
	/* nastaveni velikosti pisma */
	glScalef (0.001f, 0.001f, 1);

	/* provedeme potrebnou rotaci */
	glRotatef (180, 0, 1, 0);
	
	/* vykreslime texturu na ctvercovou plochu.*/
	glBegin(GL_QUADS);
		glTexCoord2d(1, 1); glVertex3d(X, Y, Z);
		glTexCoord2d(0, 1); glVertex3d(X+msg->w, Y, Z);
		glTexCoord2d(0, 0); glVertex3d(X+msg->w, Y+msg->h, Z);
		glTexCoord2d(1, 0); glVertex3d(X, Y+msg->h, Z);
	glEnd();
 
	/*Clean up.*/
	glDeleteTextures(1, &Texture);
	SDL_FreeSurface(msg);
} 

bool font_init ()
{
	TTF_Init ();
	
	font = TTF_OpenFont ("data/DejaVuSans.ttf", 12);

	if (!font)
		return false;

	return true;
}