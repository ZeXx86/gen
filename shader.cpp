#include "gen.h"
#include "gl.h"
#include "shader.h"
#ifdef __WIN32__
# include <io.h>
# include <sys/stat.h>
#endif

void shader_getuniform_light (GLuint prog, light_t *l)
{
	int u;
	char s[128];
	unsigned l_len = strlen (l->name);
	
	memcpy (s, l->name, l_len);
	s[l_len] = '.';
	l_len ++;
	
	memcpy (s+l_len, "position\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->position);
	
	memcpy (s+l_len, "ambient\0", 8);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->ambient);
	
	memcpy (s+l_len, "diffuse\0", 8);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->diffuse);

	memcpy (s+l_len, "specular\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->specular);

	memcpy (s+l_len, "spot_dir\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->spot_dir);

	memcpy (s+l_len, "spot_exp\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform1i (u, l->spot_exp);

	memcpy (s+l_len, "spot_cut\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform1i (u, l->spot_cut);
}

void shader_getuniform_material (GLuint prog, material_t *l)
{
	int u;
	char s[128];
	unsigned l_len = strlen (l->name);
	
	memcpy (s, l->name, l_len);
	s[l_len] = '.';
	l_len ++;
	
	memcpy (s+l_len, "ambient\0", 8);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->ambient);

	memcpy (s+l_len, "diffuse\0", 8);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->diffuse);

	memcpy (s+l_len, "specular\0", 9);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform4fv (u, 1, l->specular);

	memcpy (s+l_len, "transparency\0", 12);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform1f (u, l->transparency);

	memcpy (s+l_len, "shininess\0", 10);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform1f (u, l->shininess);

	memcpy (s+l_len, "illumination\0", 12);
	if ((u = glGetUniformLocation (prog, s)) >= 0)
		glUniform1i (u, l->illumination);
}

char *shader_load (char *file, int *len)
{
	std::ifstream is ((const char *) file);
	
	if (!is)
		return NULL;
#ifdef __WIN32__
	struct __stat64 st;
	if (_stat64 ((const char *) file, &st) != 0)
		return NULL;

	int l = st.st_size;
#else
	is.seekg (0, is.end);
	int l = is.tellg ();
	is.seekg (0, is.beg);
#endif
	char *buf = (char *) malloc (l + 1);

	is.read (buf, l);
	
	buf[l] = '\0';

	*len = l;
	
	is.close ();
    
	return buf;
}

GLuint shader_compile (char *file, int type)
{
	GLuint shader = 0;
	int len = 0;
	char *data = shader_load (file, &len);
	
	if (!data)
		return NULL;

	if (!GLEW_VERSION_3_3) {
		char *s_v = strstr (data, "#version 400 core");
		if (s_v)
			memcpy (s_v, "#version 300 es  ", 17);
		
		char *s_p = strstr (data, "//precision highp float;");
		if (s_p)
			memcpy (s_p, "precision highp float;  ", 24);
	}

	/* create shader object, set the source, and compile */
	shader = glCreateShader (type);

	glShaderSource (shader, 1, (const char **) &data, &len);
	glCompileShader (shader);
	
	free (data);

	GLsizei res;
	glGetShaderiv (shader, GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE) {
		/* get the shader info log */
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &len);
		char *log = (char *) malloc (len);
		glGetShaderInfoLog (shader, len, &res, log);

		/* print an error message and the info log */
		fprintf (stderr, "shader_compile (): Unable to compile %s: %s\n", file, log);
		free (log);

		glDeleteShader (shader);
		return NULL;
	}
	
	return shader;
}

int shader_attach (GLuint prog, char *vert_file, char *frag_file)
{
	GLuint vert = shader_compile (vert_file, GL_VERTEX_SHADER);
	
	if (!vert) {
		cerr << "File for Vertex Shader cannot be open!" << endl;
		return -1;
	}
	
	glAttachShader (prog, vert);
	/* shader neni smazan, dokud neni uvolnen program */
	glDeleteShader (vert);
	
	GLuint frag = shader_compile (frag_file, GL_FRAGMENT_SHADER);
	
	if (!frag) {
		cerr << "File for Fragment Shader cannot be open!" << endl;
		return -1;
	}
	
	glAttachShader (prog, frag);
	/* shader neni smazan, dokud neni uvolnen program */
	glDeleteShader (frag);
	
	return 0;
}

GLuint shader_init (char *file)
{
	GLuint prog;
	GLint res;

	/* create program object and attach shaders */
	prog = glCreateProgram ();

	char vert_file[80];
	sprintf (vert_file, "%s.vert", file);
	char frag_file[80];
	sprintf (frag_file, "%s.frag", file);
	
	shader_attach (prog, vert_file, frag_file);

	/* link the program and make sure that there were no errors */
	glLinkProgram (prog);
	glGetProgramiv (prog, GL_LINK_STATUS, &res);

	if (res == GL_FALSE) {
		GLint len;
		/* get the program info log */
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
		char *log = (char *) malloc (len);
		glGetProgramInfoLog (prog, len, &res, log);

		/* print an error message and the info log */
		fprintf (stderr, "shader_init(): Program linking failed: %s\n", log);
		free (log);

		/* delete the program */
		glDeleteProgram (prog);
		prog = 0;
	}
	
	return prog;
}
