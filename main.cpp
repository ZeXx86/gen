#include "gl.h"
#include "gen.h"
#include "event.h"
#include "logic.h"
#include "octree.h"
#include "camera.h"
#include "terrain.h"

SDL_Surface *g_screen;

bool init ()
{
	if (!camera_init ())	// Inicializace kamery
		return false;

	if (!terrain_init ())	// Inicializace mapy
		return false;

	if (!octree_init ())	// Inicializace Octree
		return false;

	if (SDL_Init (SDL_SUBSYSTEMS) == -1) {
		cerr << "Unable to initialize SDL: " << SDL_GetError () << endl;
		return false;
	}

	// Atributy rendering kontextu
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
#ifdef ANDROID
	SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 16);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
#else
	SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
#endif
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 0);

	SDL_GL_SetAttribute (SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute (SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute (SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute (SDL_GL_ACCUM_ALPHA_SIZE, 0);
#ifdef FSAA
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, FSAA);
#endif

	// Vytvori okno s definovanymi vlastnostmi
	g_screen = SDL_SetVideoMode (WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS);

	if (g_screen == NULL) {
		cerr << "Unable to set resolution: " << WIN_WIDTH << "x" << WIN_HEIGHT << ", with error: " << SDL_GetError () << endl;
		return false;
	}

	if (!gl_init ())	// Inicializace OpenGL
		return false;

	if (!logic_init ())
		return false;

	gl_resize (WIN_WIDTH, WIN_HEIGHT);// Nastavi perspektivu
	SDL_WM_SetCaption (WIN_TITLE, NULL);// Titulek okna

	return true;
}

void deinit ()
{
	terrain_deinit ();

	SDL_Quit ();
}

int main (int argc, char *argv[])
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

