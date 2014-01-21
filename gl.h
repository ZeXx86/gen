#ifndef __gl_h__
#define __gl_h__

#include "terrain.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#define FPS_MAX		60.0f
#define NEAR_PLANE 	0.15
#define FAR_PLANE 	500.0

extern SDL_mutex *gl_mutex;

extern bool gl_init ();
extern bool gl_prepare_terrain (terrain_t *t);
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

