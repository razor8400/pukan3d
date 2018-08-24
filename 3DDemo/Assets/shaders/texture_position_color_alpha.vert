#version 330 core
layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;
layout(location = 2) in vec4 vertex_color;

out vec2 uv;
out vec4 color;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(vertex_position_modelspace, 1);
    uv = vertex_uv;
    color = vertex_color;
}