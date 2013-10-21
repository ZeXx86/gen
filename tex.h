#ifndef __tex_h__
#define __tex_h__

#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern GLuint tex_get (unsigned id);
extern bool tex_init ();
extern void tex_deinit ();

#endif

