#include "gl.h"
#include "gen.h"
#include "vbo.h"


// Funkce pro alokaci Vertex Buffer Object
unsigned vbo_alloc (void *buf, unsigned long len)
{
	GLuint id = 0;
	
	glGenBuffers (1, &id);
	glBindBuffer (GL_ARRAY_BUFFER, id);
	glBufferData (GL_ARRAY_BUFFER, len, buf, GL_STATIC_DRAW);

   	return id;
}

void vbo_free (unsigned *id)
{	
	glDeleteBuffers (1, (GLuint *) id);
	*id = 0;
}
