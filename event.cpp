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
			// Klavesnice
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

			// Zmena velikosti okna
			case SDL_VIDEORESIZE:
#ifdef CALL_SETVIDEOMODE_WHEN_RESIZING
				g_screen = SDL_SetVideoMode (event.resize.w, event.resize.h, WIN_BPP, WIN_FLAGS);

				if (g_screen == NULL) {
					//fprintf (stderr, "Unable to resize window: %s.\n", SDL_GetError ());
					return false;
				}
#endif
				gl_resize (event.resize.w, event.resize.h);
				break;
			// Pozadavek na ukonceni
			case SDL_QUIT:
				return false;
				break;

			default:
				break;
		}
	}

	return true;
}

