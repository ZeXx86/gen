#version 400 core

//precision highp float;

struct LightInfo {
	vec4 position;		//Light position   X,Y,Z, directional=0 OR positional=1
	vec4 ambient;		//Light ambient color
	vec4 diffuse;		//Light diffuse color
	vec4 specular;		//Light specular color
	vec4 spot_dir;		//Specifies light direction
	int spot_exp;		//Specifies intensity distribution of spotlight
	int spot_cut;		//Specifies maximum spread angle of spotlight (180 = off).
};

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;
layout(location = 3) in float VertexParam;

uniform mat4 PMatrix;
uniform mat4 VMatrix;
uniform mat4 MVMatrix;
uniform mat3 NormalMatrix;

uniform LightInfo light;

out vec4 ecPosition;			
out vec3 ecLightDir;
out vec3 ecNormal;
out vec3 ecViewDir;
out vec4 projCoord;
out float voxel_val;
out vec2 UV;

/* pseudonahodna funkce pro generovani sumu */
float rand (vec3 co)
{
	return fract (sin (dot (co.xyz ,vec3 (12.9898,78.233, 644.631))) * 43758.5453);
}

void main ()
{
	float random = rand (VertexPosition);

	vec4 pos = vec4 (VertexPosition.x, VertexPosition.y+random, VertexPosition.z, 1);

	if (mod (VertexPosition.x, 32) == 0 || mod (VertexPosition.y, 32) == 0 || mod (VertexPosition.z, 32) == 0)
		pos = vec4 (VertexPosition, 1.0);
	
	ecPosition = MVMatrix * pos;
	ecLightDir = vec3 (VMatrix * light.position - ecPosition);
	ecNormal = NormalMatrix * (vec3 (VertexNormal.x/3, VertexNormal.y/3, VertexNormal.z/3)) + random;
	ecViewDir = -vec3 (ecPosition);
	
	gl_Position = PMatrix * ecPosition;
	
	voxel_val = VertexParam;
	projCoord = pos;
	UV = VertexTexCoord;
}
