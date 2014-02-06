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

#include "gen.h"
#include "mouse.h"

static mouse_t mouse;

#ifdef ANDROID
int mouse_last_pos[2] = { 0, 0 };
int mouse_abs_pos[2] = { 0, 0 };
int mouse_ang_pos[2] = { 0, 0 };
unsigned char mouse_switch = 0;

extern "C" {
	JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_onNativeTouch (JNIEnv *env, jobject obj, jint devid, jint pfid, jint act, jfloat x, jfloat y, jfloat p);
};

JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_onNativeTouch (JNIEnv *env, jobject obj, jint devid, jint pfid, jint act, jfloat x, jfloat y, jfloat p)
{
	mouse.state = act;
	int m_x = (int) (x*32768);
	int m_y = (int) (y*32768);

	if (act == 0) {
		mouse_last_pos[0] = m_x;
		mouse_last_pos[1] = m_y;
	}

	mouse_abs_pos[0] = m_x;
	mouse_abs_pos[1] = m_y;
}
#endif

mouse_t *mouse_get ()
{
#ifndef ANDROID
	mouse.state = SDL_GetRelativeMouseState (&mouse.x, &mouse.y);
#else
	mouse.x = (mouse_abs_pos[0] - mouse_last_pos[0]) / 20;
	mouse.y = (mouse_abs_pos[1] - mouse_last_pos[1]) / 20;

	mouse.abs_x = mouse_abs_pos[0];
	mouse.abs_y = mouse_abs_pos[1];

	mouse_last_pos[0] = mouse_abs_pos[0];
	mouse_last_pos[1] = mouse_abs_pos[1];
#endif
	return &mouse;
}
