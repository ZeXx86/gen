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
#include "event.h"
#include "kbd.h"
#include "gl.h"

bool event_handler ()
{
	SDL_Event event;
	
	while (SDL_PollEvent (&event)) {
		switch (event.type) {
			case SDL_MOUSEMOTION:
			 	//x = event.motion.x;
				//y = event.motion.y;
				break;
			/* Klavesnice - obsluha stisku klaves */
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				kbd_handler (event);

				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						return false;
						break;

					default:
						break;
				}
				break;
			/* Zmena velikosti okna - od SDL2 se resi jinak */
/*			case SDL_VIDEORESIZE:
				
#ifdef CALL_SETVIDEOMODE_WHEN_RESIZING
				g_screen = SDL_SetVideoMode (event.resize.w, event.resize.h, WIN_BPP, WIN_FLAGS);

				if (g_screen == NULL) {
					//fprintf (stderr, "Unable to resize window: %s.\n", SDL_GetError ());
					return false;
				}
#endif
				gl_resize (event.resize.w, event.resize.h);
				break;*/
			/* Pozadavek na ukonceni */
			case SDL_QUIT:
				return false;	// vracime FALSE, ukoncime hlavni smycku
				break;
			default:
				break;
		}
	}

	return true;
}

