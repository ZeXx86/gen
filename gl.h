#ifndef __gl_h__
#define __gl_h__

#include "terrain.h"
#include <SDL2/SDL.h>

#ifdef ANDROID
# include <SDL2/SDL_opengles.h>

# define GLdouble GLfloat
#else
# include <SDL2/SDL_ttf.h>
# include <SDL2/SDL_opengl.h>
# include <GL/glu.h>
#endif

#define FPS_MAX		60.0f
#define NEAR_PLANE 	0.15
#ifndef ANDROID
# define FAR_PLANE 	500.0
#else
# define FAR_PLANE 	150.0
#endif

extern SDL_mutex *gl_mutex;

extern bool gl_init ();
extern bool gl_prepare_terrain (terrain_t *t);
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

