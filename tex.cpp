#include "gen.h"
#include "tex.h"

static unsigned tex_cnt;
static GLuint *tex_list;

static SDL_Surface *LoadBitmap (const char *filename)// Funkce pro načteni bitmapy
{
	unsigned char *rowhi, *rowlo;// Ukazatele na prohazováni řádků
	unsigned char *tmpbuf, tmpch;// Dočasná paměť
	int i, j;// Řídící proměnné pro cykly
	SDL_Surface *image;// Načítaný obrázek

	image = IMG_Load (filename);// Načtení dat obrázku

	if (image == NULL) {
		fprintf (stderr, "Nepodarilo se nacist %s: %s\n", filename, SDL_GetError());
		return 0;
	}

	// GL surfaces jsou vzhuru nohama - budeme nasi bitmapu muset prevratit
	tmpbuf = (Uint8 *) malloc (image->pitch);

	if (tmpbuf == NULL) {
		fprintf (stderr, "Nedostatek pameti\n");
	        return NULL;
	}

	// nastaveni prvniho a posledniho radku
	rowhi = (Uint8 *) image->pixels;
	rowlo = rowhi + (image->h * image->pitch) - image->pitch;
	   
	for (i = 0; i < image->h / 2; i ++) {
		// prevraceni BGR na RGB
	      	if (image->format->Bshift == 0)
			for (j = 0; j < image->w; j ++) {
		    		tmpch = rowhi[j*3];
		    		rowhi[j*3] = rowhi[j*3+2];
		    		rowhi[j*3+2] = tmpch;
		    		tmpch = rowlo[j*3];
		    		rowlo[j*3] = rowlo[j*3+2];
		    		rowlo[j*3+2] = tmpch;
			}

	      	// prohozeni radku
	      	memcpy (tmpbuf, rowhi, image->pitch);
	      	memcpy (rowhi,  rowlo, image->pitch);
	      	memcpy (rowlo, tmpbuf, image->pitch);
	      
	      	// posun ikazatelu na radky
	      	rowhi += image->pitch;
	      	rowlo -= image->pitch;
	}

	// uklidime
	free (tmpbuf);

	return image;
}

// Funkce pro konverzi bitmapy do textury
static bool tex_create (const char *file)
{
	// nacteni obrazku
   	SDL_Surface *surface;
   	surface = LoadBitmap (file);

   	if (!surface) 
		return 0;

   	// vytvoreni objektu textury
   	GLuint texture;

   	// vygenerovani textury
   	glGenTextures (1, &texture);

   	// nastaveni textury
   	glBindTexture (GL_TEXTURE_2D, texture);

   	// predani parametru
   	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   	// pridani barevne mapy (obrazku) - pripadne generovani mip map
   	//if (1) 
      		gluBuild2DMipmaps (GL_TEXTURE_2D, 3, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
   	//else
      	//	glTexImage2D (GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

   	// uklid - smazani SDL_Surface
   	SDL_FreeSurface (surface);
   	surface = 0;

	GLuint *list = (GLuint *) realloc (tex_list, sizeof (GLuint) * (tex_cnt+1));
	
	if (list) {
		memcpy (list, tex_list, sizeof (GLuint) * tex_cnt);
		tex_list = list;

		tex_list[tex_cnt] = texture;

		tex_cnt ++;
	}

   	return texture;
}

GLuint tex_get (unsigned id)
{
	if (id >= tex_cnt)
		return 0;

	return tex_list[id];
}

bool tex_init ()
{
	tex_cnt = 0;
	tex_list = 0;

	tex_create ("./data/grass.jpg");
	//tex_create ("./data/oct20clava.jpg");

	return true;
}

void tex_deinit ()
{
	for (unsigned i = 0; i < tex_cnt; i ++)
		glDeleteTextures (1, &tex_list[i]);

	free (tex_list);
}

