#include "gen.h"
#include "mouse.h"

static mouse_t mouse;

mouse_t *mouse_get ()
{
	mouse.state = SDL_GetRelativeMouseState (&mouse.x, &mouse.y);

	return &mouse;
}

