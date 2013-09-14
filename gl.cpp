#include "gl.h"
#include "gen.h"
#include "octree.h"
#include "camera.h"
#include "terrain.h"

static float fps_stick, fps_dtick;

#define RENDER_TERRAIN	1

bool gl_init ()
{
	glClearColor (0.4, 0.4, 1.0, 0.0);
	glClearDepth (1.0);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable (GL_TEXTURE_2D);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);
	
	//glBlendColor(0.5f, 0.5f, 0.5f, 0.5f);

	return true;
}

void gl_resize (int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport (0, 0, width, height);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (60.0, (GLdouble) width / (GLdouble) height, NEAR_PLANE, FAR_PLANE);

	glMatrixMode (GL_MODELVIEW);
}

float gl_fps_get ()
{
	if (!fps_dtick)
		return 1000.0f;

	return 1000.0f / fps_dtick;
}

void gl_render_octree_mesh (octree_t *node)
{
	terrain_t *t = terrain_get ();

	if (node->size_x == 2) {
	
	glPushMatrix ();
		glTranslatef (node->origin_x*TERRAIN_VOXEL_DIM-1, node->origin_z*TERRAIN_VOXEL_DIM-1, node->origin_y*TERRAIN_VOXEL_DIM-1);
			
		glScalef (node->size_x*1.001f, node->size_z*1.001f, node->size_y*1.001f);
		
		float c_q = 255;

		glBegin(GL_QUADS);
		   // Front Face
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
		   // Back Face
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
		   // Top Face
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1, -1.0f);  // Top Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1,  1.0f);  // Bottom Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1,  1.0f);  // Bottom Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1, -1.0f);  // Top Right Of The Texture and Quad
		   // Bottom Face
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		   // Right face
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		   // Left Face
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
		   glColor4ub(0, c_q, 0, 60); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
		glEnd();
	
		glPopMatrix ();
	}

	for (int i = 0; i < 8; i ++) {
		if (node->children[i])
			gl_render_octree_mesh (node->children[i]);	
	}
}




void gl_render_terrain (octree_t *node)
{
	for (int i = 0; i < 8; i ++) {
		if (node->children[i])
			gl_render_terrain (node->children[i]);	
	}

	terrain_t *t = terrain_get ();
#ifdef RENDER_TERRAIN
	if (node->size_x != 1)
		return;

	unsigned x = node->origin_x-1;
	unsigned y = node->origin_y-1;
	unsigned z = node->origin_z-1;
		
	//voxel_t *v = terrain_voxel_get (x, y, z);

	glPushMatrix ();
	glTranslatef (x*TERRAIN_VOXEL_DIM, z*TERRAIN_VOXEL_DIM, y*TERRAIN_VOXEL_DIM);

	voxel_t *v_b[9];
	v_b[0] = terrain_voxel_get (x-1, y-1, z-1);
	v_b[1] = terrain_voxel_get (x, y-1, z-1);
	v_b[2] = terrain_voxel_get (x+1, y-1, z-1);
	v_b[3] = terrain_voxel_get (x-1, y, z-1);
	v_b[4] = terrain_voxel_get (x, y, z-1);
	v_b[5] = terrain_voxel_get (x+1, y, z-1);
	v_b[6] = terrain_voxel_get (x-1, y+1, z-1);
	v_b[7] = terrain_voxel_get (x, y+1, z-1);
	v_b[8] = terrain_voxel_get (x+1, y+1, z-1);

	voxel_t *v_c[9];
	v_c[0] = terrain_voxel_get (x-1, y-1, z);
	v_c[1] = terrain_voxel_get (x, y-1, z);
	v_c[2] = terrain_voxel_get (x+1, y-1, z);
	v_c[3] = terrain_voxel_get (x-1, y, z);
	v_c[4] = terrain_voxel_get (x, y, z);
	v_c[5] = terrain_voxel_get (x+1, y, z);
	v_c[6] = terrain_voxel_get (x-1, y+1, z);
	v_c[7] = terrain_voxel_get (x, y+1, z);
	v_c[8] = terrain_voxel_get (x+1, y+1, z);
	
	voxel_t *v_t[9];
	v_t[0] = terrain_voxel_get (x-1, y-1, z+1);
	v_t[1] = terrain_voxel_get (x, y-1, z+1);
	v_t[2] = terrain_voxel_get (x+1, y-1, z+1);
	v_t[3] = terrain_voxel_get (x-1, y, z+1);
	v_t[4] = terrain_voxel_get (x, y, z+1);
	v_t[5] = terrain_voxel_get (x+1, y, z+1);
	v_t[6] = terrain_voxel_get (x-1, y+1, z+1);
	v_t[7] = terrain_voxel_get (x, y+1, z+1);
	v_t[8] = terrain_voxel_get (x+1, y+1, z+1);
	
	float c_q = (v_c[4]->value + 1) * 127;

	if (v_c[4] < 0) {
		glPopMatrix ();
		return;
	}
	
	/* do not process voxel inside a voxelo-polygon */
	if ((v_c[3]->value > 0.0f && v_c[5]->value > 0.0f && v_c[1]->value > 0.0f &&
		v_c[7]->value > 0.0f && v_b[4]->value > 0.0f && v_t[4]->value > 0.0f)) {
		glPopMatrix ();

		return;
	}

	voxel_t **v_x = v_t;
	float s = 1.0f;	
		
	while (1) {
		if (v_c[4]->value > 0 && v_x[4]->value <= 0) {
			if (v_x[5]->value > 0) {

				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3, 1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, 1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();
			} else if (v_x[3]->value > 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, 1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, 1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();
			} else if (v_x[7]->value > 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, 1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*1, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*3, 1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();
			} else if (v_x[1]->value > 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();	

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();

				glBegin(GL_TRIANGLES);
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*3, -1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*1, -1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(1.0f, s*1, 1.0f);  // Top Right Of The Texture and Quad
				glEnd();
			} else if (v_x[8]->value > 0 && v_x[7]->value <= 0 && v_x[5]->value <= 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();	
			} else if (v_x[2]->value > 0 && v_x[5]->value <= 0 && v_x[1]->value <= 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*3, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();	
			} else if (v_x[0]->value > 0 && v_x[1]->value <= 0 && v_x[3]->value <= 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();	
			} else if (v_x[6]->value > 0 && v_x[7]->value <= 0 && v_x[3]->value <= 0) { 
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*3,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();	
			} else {
				glBegin(GL_QUADS);
					// horizontal Face top
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1, -1.0f);  // Top Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, s*1,  1.0f);  // Bottom Left Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1,  1.0f);  // Bottom Right Of The Texture and Quad
					glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, s*1, -1.0f);  // Top Right Of The Texture and Quad
				glEnd();
			}
		}
		
		// switch to bottom
		if (s == -1.0f)
			break;
		
		s = -1.0f;
		v_x = v_b;
	}
	
	// Back
	if (v_c[4]->value > 0 && v_c[1]->value <= 0) {
		glBegin(GL_QUADS);
		    // Back Face
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
		glEnd();
	}
	
	// Front
	if (v_c[4]->value > 0 && v_c[7]->value <= 0) {
		glBegin(GL_QUADS);
		    // Front Face
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
		glEnd();
	}
	
	// Left
	if (v_c[4]->value > 0 && v_c[3]->value <= 0) {
		glBegin(GL_QUADS);
		    // Left Face
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
		glEnd();
	}
	
	// Right
	if (v_c[4]->value > 0 && v_c[5]->value <= 0) {
		glBegin(GL_QUADS);
		    // Right face
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
		    glColor4ub(0, c_q, 0, 255); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
		glEnd();
	}

	glPopMatrix ();	
#endif

}

void gl_render ()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	camera_t *c = camera_get ();

	/* scene motion */
	glRotatef (c->rot_y, 1, 0, 0);
	glRotatef (c->rot_x, 0, 1, 0);
	glTranslatef (c->pos_x, c->pos_z, c->pos_y);

	octree_t *root = octree_root_get ();

	gl_render_terrain (root);
	
	//gl_render_octree_mesh (root);

	glFlush ();
	SDL_GL_SwapBuffers ();// Prohodi predni a zadni buffer

	fps_dtick = SDL_GetTicks () - fps_stick;

	if (1000/FPS_MAX > fps_dtick)
		SDL_Delay (1000/FPS_MAX - fps_dtick);

	fps_stick = SDL_GetTicks ();
}

