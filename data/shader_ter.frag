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

struct MaterialInfo {
	vec4 ambient;			//Material ambient reflectivity
	vec4 diffuse;			//Material diffuse reflectivity
	vec4 specular;			//Material specular reflectivity
	float transparency;		//Material transparency factor
	float shininess;		//Material shininess
	int illumination;
};

layout(location = 0) out vec4 FragColor;

uniform LightInfo light;
uniform MaterialInfo material;

// Values that stay constant for the whole mesh.
uniform sampler2D tex_sampler0;
uniform sampler2D tex_sampler1;
uniform sampler2D tex_sampler2;

//The prefix ec means Eye Coordinates in the Eye Coordinate System
in vec4 ecPosition;			
in vec3 ecLightDir;
in vec3 ecNormal;
in vec3 ecViewDir;

// Interpolated values from the vertex shaders
in vec2 UV;
in float voxel_val;

in vec4 projCoord;

float rand (vec3 co)
{
	return fract (sin (dot (co.xyz ,vec3 (12.9898,78.233, 644.631))) * 43758.5453);
}

void main()
{
	vec4 texColor;
	
	//texColor = texture (tex_sampler0, UV);
	texColor = vec4 (mix (texture (tex_sampler0, UV).rgb, texture (tex_sampler1, UV).rgb, voxel_val / 96), 1.0);
	
	/*if (voxel_val > 32 && voxel_val < 64)
		texColor = vec4 (texture (tex_sampler1, UV).rgb, 1.0);
	else if (voxel_val >= 64)
		texColor = vec4 (texture (tex_sampler2, UV).rgb, 1.0);*/
		
	vec3 N = ecNormal;// + (2.0 * texture (TexBump, UV).rgb - 1.0);
	N = normalize (N); 
	vec3 L = normalize (ecLightDir);
	

	FragColor = light.ambient  * material.ambient;  
	float lambert = dot (N,L);
	
	if (lambert > 0.0) {
		FragColor += light.diffuse * material.diffuse * lambert;
		vec3 E = normalize(ecViewDir);
		vec3 R = normalize( 2.0 * dot(N, ecLightDir) * N - ecLightDir); 
		float specular = pow(max(dot(R,E), 0.0), 20.0 );
		FragColor += light.specular* material.specular * specular;
	}

	FragColor *= texColor;

}
