#include "gl.h"
#include "gen.h"
#include "tex.h"
#include "font.h"
#include "water.h"
#include "event.h"
#include "logic.h"
#include "octree.h"
#include "camera.h"
#include "frustum.h"
#include "terrain.h"

SDL_Window *g_window;

bool init ()
{
	if (!camera_init ())	// Inicializace kamery
		return false;
	
	if (SDL_Init (SDL_SUBSYSTEMS) == -1) {
		cerr << "Unable to initialize SDL: " << SDL_GetError () << endl;
		return false;
	}
	// Vytvori okno s definovanymi vlastnostmi
	g_window = SDL_CreateWindow (WIN_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						WIN_WIDTH, WIN_HEIGHT, WIN_FLAGS);
	
	if (g_window == NULL) {
		cerr << "Unable to set resolution: " << WIN_WIDTH << "x" << WIN_HEIGHT << ", with error: " << SDL_GetError () << endl;
		return false;
	}

	// Vytvorime SDL GL Context
  	SDL_GLContext glcontext = SDL_GL_CreateContext (g_window);
	
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifndef ANDROID
	SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
#endif

#ifdef FSAA
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, FSAA);
#endif	
	
	// VSync
	//SDL_GL_SetSwapInterval (1);
	if (!terrain_init ())	// Inicializace terenu
		return false;

	if (!octree_init ())	// Inicializace Octree
		return false;
	
	if (!font_init ())
		return false;	// Inicializace fontu

	if (!gl_water_init ())
		return false;

	if (!gl_init ())	// Inicializace OpenGL
		return false;

	if (!tex_init ())	// Inicializace textur
		return false;

	if (!logic_init ())
		return false;

	gl_resize (WIN_WIDTH, WIN_HEIGHT);// Nastavi perspektivu
	//SDL_WM_SetCaption (WIN_TITLE, NULL);// Titulek okna
	//gl_resize (WIN_WIDTH, WIN_HEIGHT);// Nastavi perspektivu

#ifndef ANDROID
	SDL_ShowCursor (SDL_DISABLE);
	SDL_SetWindowGrab (g_window, SDL_TRUE);
	SDL_SetRelativeMouseMode (SDL_TRUE);
#endif
	return true;
}

void deinit ()
{
	octree_deinit ();
	
	terrain_deinit ();
	
	tex_deinit ();

	SDL_Quit ();
}

int SDL_main (int argc, char *argv[])
{
	if (!init ())
		return -1;

	bool f = false;
	while (!f) {
		f = !event_handler (); 	// Osetri udalosti

		gl_render ();		// Rendering
	}

	deinit ();

	return 0;
}

