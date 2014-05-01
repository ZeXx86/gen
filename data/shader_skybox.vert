#version 400 core

//precision highp float;

layout(location = 0) in vec3 VertexPosition;

uniform mat4 MVMatrix;		// Model View Matrix
uniform mat4 PMatrix;		// Projection Matrix

out vec3 UV;

void main ()
{
	mat4 r = MVMatrix;
	r[3][0] = 0.0;
	r[3][1] = 0.0;
	r[3][2] = 0.0;
	
	vec4 v = inverse(PMatrix * r) * vec4 (VertexPosition, 1.0);

	UV = vec3 (v.xy, v.z); 
	gl_Position = vec4 (VertexPosition, 1.0);
}
