#version 400 core

//precision highp float;

layout(location = 0) out vec4 FragColor;

uniform sampler2D tex_sampler0;

in vec2 UV;

void main()
{
	FragColor = vec4 (texture (tex_sampler0, UV*50).rgb, 0.5);
}
