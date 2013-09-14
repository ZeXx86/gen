#ifndef __kbd_h__
#define __kbd_h__

#include <SDL/SDL.h>

typedef struct {
	SDL_Event event;
} kbd_t;

extern void kbd_handler (SDL_Event event);
extern bool kbd_key_pressed (unsigned short key);

#endif

