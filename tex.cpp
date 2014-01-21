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
static GLuint tex_create (const char *file)
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
      		//gluBuild2DMipmaps (GL_TEXTURE_2D, 3, surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
   	//else
      		glTexImage2D (GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

   	// uklid - smazani SDL_Surface
   	SDL_FreeSurface (surface);
   	surface = 0;

	GLuint *list = (GLuint *) realloc (tex_list, sizeof (GLuint) * (tex_cnt+1));
	
	if (list) {
		tex_list = list;

		tex_list[tex_cnt] = texture;

		tex_cnt ++;
	}

   	return texture;
}

// Funkce pro konverzi bitmapy do textury s alfou
static GLuint tex_create_alpha (const char *file, const char *file2)
{
	// nacteni obrazku
   	SDL_Surface *surface, *surface2;

	//SDL_Surface *image;// Načítaný obrázek

	surface = IMG_Load (file);	// Načtení dat obrázku
	surface2 = IMG_Load (file2);	// Načtení dat obrázku

   	if (surface == NULL || surface2 == NULL) 
		return 0;
	
	unsigned char total_texture[4 * 256 * 256];
	
	unsigned char *s = (unsigned char *) surface->pixels;
	unsigned char *a = (unsigned char *) surface2->pixels;
	
	for (int i = 0; i < surface->w * surface->h; i++)
	{
		total_texture[4 * i] = s[3 * i];
		total_texture[4 * i + 1] = s[3 * i + 1];
		total_texture[4 * i + 2] = s[3 * i + 2];
		total_texture[4 * i + 3] = a[i];
	}
	
 	// vytvoreni objektu textury
   	GLuint texture;
	
	glGenTextures (1, &texture);
	
	glBindTexture (GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, total_texture);
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glEnable (GL_TEXTURE_GEN_S);
	//glEnable (GL_TEXTURE_GEN_T);
	//glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  
   	// uklid - smazani SDL_Surface
   	SDL_FreeSurface (surface);
	SDL_FreeSurface (surface2);
   	surface = 0;

	GLuint *list = (GLuint *) realloc (tex_list, sizeof (GLuint) * (tex_cnt+1));
	
	if (list) {
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
	
	tex_create ("./data/skybox/negz.jpg");
	tex_create ("./data/skybox/posx.jpg");
	tex_create ("./data/skybox/posz.jpg");
	tex_create ("./data/skybox/negx.jpg");
	tex_create ("./data/skybox/posy.jpg");
	tex_create ("./data/skybox/negy.jpg");
	
	tex_create_alpha ("./data/water.jpg", "./data/alpha.jpg");

	
	//tex_create ("./data/snow.jpg");
	//tex_create ("./data/oct20clava.jpg");
	


	return true;
}

void tex_deinit ()
{
	for (unsigned i = 0; i < tex_cnt; i ++)
		glDeleteTextures (1, &tex_list[i]);

	free (tex_list);
}

