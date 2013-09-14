#ifndef __gen_h__
#define __gen_h__

#include <math.h>
#include <iostream>
#include <SDL/SDL.h>

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#ifndef ANDROID
# define WIN_FLAGS SDL_OPENGL | SDL_RESIZABLE/* | SDL_FULLSCREEN*/
# define WIN_WIDTH 1600
# define WIN_HEIGHT 1024
# define WIN_BPP 0
# define FSAA 2
#else
# define WIN_FLAGS SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN
# define WIN_WIDTH 800
# define WIN_HEIGHT 480
# define WIN_BPP 0
# define FSAA 0
#endif

#define WIN_TITLE "OpenGL/Voxel Terrain Generator"


#ifndef __WIN32__
# define CALL_SETVIDEOMODE_WHEN_RESIZING
#endif

using namespace std;

extern SDL_Surface *g_screen;

#endif

