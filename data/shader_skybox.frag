#version 400 core

//precision highp float;

layout(location = 0) out vec4 FragColor;

uniform samplerCube tex_sampler0;

in vec3 UV;

void main()
{
	FragColor = texture (tex_sampler0, UV);
}
