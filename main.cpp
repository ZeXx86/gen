#include "gl.h"
#include "gen.h"
#include "vbo.h"
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
	if (SDL_Init (SDL_SUBSYSTEMS) == -1) {
		cerr << "Unable to initialize SDL: " << SDL_GetError () << endl;
		return false;
	}
	
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef FSAA
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, FSAA);
#endif	
  	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 1);

	SDL_DisplayMode current;

	// Get current display mode of all displays.
	for (int i = 0; i < SDL_GetNumVideoDisplays (); ++ i) {  
		int ret = SDL_GetCurrentDisplayMode(i, &current);

	    	if (ret != 0)
	    	  	printf ("Could not get display mode for video display #%d: %s", i, SDL_GetError ());
	    	else 
	      		printf ("Display #%d: current display mode is %dx%dpx @ %dhz. \n", i, current.w, current.h, current.refresh_rate);

	}
	
	// Vytvori okno s definovanymi vlastnostmi
	g_window = SDL_CreateWindow (WIN_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
						current.w, current.h, WIN_FLAGS);
	
	if (g_window == NULL) {
		cerr << "Unable to set resolution: " << current.w << "x" << current.h << ", with error: " << SDL_GetError () << endl;
		return false;
	}

	// Vytvorime SDL GL Context
  	SDL_GLContext glcontext = SDL_GL_CreateContext (g_window);
	
	if (!glcontext)
		return false;

	int glewStatus = glewInit();

	if (glewStatus != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString (glewStatus) << endl;
		return false;
	}
	
	if (!GLEW_VERSION_3_3) {
		printf ("WARNING -> your graphic card does not support OpenGL 3.3+\n");
		printf ("-> switching to GLSL 3.0 ES\n");
	}

	// VSync
	//SDL_GL_SetSwapInterval (1);

	if (!terrain_init ())	// Inicializace terenu
		return false;

	if (!octree_init ())	// Inicializace Octree
		return false;
	
	if (!font_init ())
		return false;	// Inicializace fontu

	if (!gl_init ())	// Inicializace OpenGL sceny
		return false;

	if (!tex_init ())	// Inicializace textur
		return false;
	
	if (!gl_water_init ())
		return false;

	if (!logic_init ())	// Inicializace logiky programu
		return false;

	gl_resize (current.w, current.h);// Nastavi perspektivu
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

