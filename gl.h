#ifndef __gl_h__
#define __gl_h__

#include "terrain.h"
#include <SDL2/SDL.h>

#ifdef ANDROID
# include <SDL2/SDL_opengles.h>

# define GLdouble GLfloat
#else
# include <GL/glew.h>
# include <GL/gl.h>
# include <SDL2/SDL_ttf.h>
# include <SDL2/SDL_opengl.h>

#endif

#define FPS_MAX		60.0f

extern SDL_mutex *gl_mutex;

extern bool gl_init ();
extern bool gl_prepare_terrain (terrain_t *t);
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

