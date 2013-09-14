#include "gen.h"
#include "kbd.h"

static kbd_t kbd;

static bool kbmap[323] = { false };

void kbd_handler (SDL_Event event)
{
	kbd.event = event;

	if (event.type == SDL_KEYDOWN)
		kbmap[event.key.keysym.sym] = true;
	if (event.type == SDL_KEYUP)
		kbmap[event.key.keysym.sym] = false;
}

bool kbd_key_pressed (unsigned short key)
{
	return kbmap[key];
}

