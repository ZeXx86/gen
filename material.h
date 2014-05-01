#ifndef __material_h__
#define __material_h__

typedef struct {
	float ambient[4];
	float diffuse[4];
	float specular[4];	
	float transparency;
	float shininess;
	int illumination;
	
	char *name;
} material_t;

#endif

