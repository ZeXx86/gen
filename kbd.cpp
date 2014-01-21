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
#include "kbd.h"

static kbd_t kbd;

static bool kbmap[323] = { false };

void kbd_handler (SDL_Event event)
{
	if (event.key.keysym.sym >= sizeof (kbmap))
		return;
	
	kbd.event = event;

	/* znacime si stisk a pusteni klaves do mapy,
	 * abychom mohli pracovat teoreticky se vsemi klavesy naraz */
	if (event.type == SDL_KEYDOWN)
		kbmap[event.key.keysym.sym] = true;
	if (event.type == SDL_KEYUP)
		kbmap[event.key.keysym.sym] = false;
}

bool kbd_key_pressed (unsigned short key)
{
	return kbmap[key];
}

