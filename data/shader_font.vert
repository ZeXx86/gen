#version 400 core

//precision highp float;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 VertexTexcoord;

uniform mat4 MVMatrix;		// Model View Matrix
uniform mat4 PMatrix;		// Projection Matrix

out vec2 UV;

void main ()
{
	UV = VertexTexcoord; 

	gl_Position = PMatrix * MVMatrix * vec4 (VertexPosition, 1.0);
}
