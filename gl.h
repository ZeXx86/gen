#ifndef __gl_h__
#define __gl_h__

#include <SDL/SDL_opengl.h>

#define FPS_MAX		60.0f
#define NEAR_PLANE 	0.15
#define FAR_PLANE 	500.0

extern bool gl_init ();
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

