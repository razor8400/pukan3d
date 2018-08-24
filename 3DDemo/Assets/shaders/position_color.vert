#version 330 core
layout(location = 0) in vec3 vertex_position_modelspace;
layout (location = 2) in vec3 vertex_color;

uniform mat4 mvp;
out vec3 color;

void main()
{
    gl_Position = mvp * vec4(vertex_position_modelspace, 1);
    color = vertex_color;
}