#version 330 core
out vec3 fragment_color;
in vec3 color;

void main()
{
    fragment_color = color;
}