#ifndef __camera_h__
#define __camera_h__

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NEAR_PLANE 	0.15f
#ifndef ANDROID
# define FAR_PLANE 	800.0f
#else
# define FAR_PLANE 	150.0f
#endif

#define CAMERA_SPEED			1.0f / 25
#define CAMERA_STATE_LMOUSECLICK	0x1
#define CAMERA_STATE_RMOUSECLICK	0x4

typedef struct {
	int viewport_x;
	int viewport_y;

	int window_width;
	int window_height;

	double aspect;
	float fov;
	double near_clip;
	double far_clip;
	
	unsigned state;
	
	float rot_x;
	float rot_y;
	float rot_dx;
	float rot_dy;

	glm::vec3 pos;
	
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 MVP;
} camera_t;

extern camera_t *camera_get ();
extern void camera_position (glm::vec3 pos);
extern void camera_update ();
extern bool camera_init (int viewport_x, int viewport_y, int window_width, int window_height);
extern void camera_deinit ();


#endif

