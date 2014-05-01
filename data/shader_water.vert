#version 400 core

//precision highp float;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexcoord;

uniform mat4 MVMatrix;		// Model View Matrix
uniform mat4 PMatrix;		// Projection Matrix
uniform vec3 cam_pos;
uniform float time_wave;

out vec2 UV;
out vec3 vert_pos;

void main ()
{
	UV = VertexTexcoord + time_wave - vec2 (cam_pos.x, cam_pos.z); 
	vert_pos = VertexPosition;
	
	gl_Position = PMatrix * MVMatrix * vec4 (VertexPosition, 1.0);
}
