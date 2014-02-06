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

#ifndef __gen_h__
#define __gen_h__

#include <math.h>
#include <iostream>
#include <SDL2/SDL.h>

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO

#ifndef ANDROID	/* nastaveni zobrazeni pro platformu PC */
# define WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE// | SDL_WINDOW_FULLSCREEN_DESKTOP
# define WIN_WIDTH 1280
# define WIN_HEIGHT 1024
# define WIN_BPP 0
# define FSAA 1


# define LOGI printf
# define SDL_main main
#endif

#ifdef ANDROID /* nastaveni zobrazeni pro Android */
# define WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
# define WIN_WIDTH 480
# define WIN_HEIGHT 800
# define WIN_BPP 0
# define FSAA 0

# include <jni.h>
# include <android/log.h>
# define  LOG_TAG    "Generator"
# define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif

#ifdef ANDROID
# define PATH_DATA	"/sdcard/gen/data/"
#else
# define PATH_DATA	"./data/"
#endif

#define WIN_TITLE "OpenGL/Voxel Terrain Generator"

using namespace std;

extern SDL_Window *g_window;

#endif

