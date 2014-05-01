#ifndef __light_h__
#define __light_h__

typedef struct {
	float position[4];	//Light position   X,Y,Z, directional=0 OR positional=1

	float ambient[4];	//Light ambient color
	float diffuse[4];	//Light diffuse color
	float specular[4];	//Light specular color
	
	float spot_dir[4];	//Specifies light direction
	int spot_exp;		//Specifies intensity distribution of spotlight
	int spot_cut;		//Specifies maximum spread angle of spotlight (180 = off)
	
	char *name;
} light_t;

#endif

