#version 330 core
in vec2 uv;
in vec4 color;

out vec4 fragment_color;
uniform sampler2D sampler2d;

void main()
{
	fragment_color = texture(sampler2d, uv).rgba * color;
}