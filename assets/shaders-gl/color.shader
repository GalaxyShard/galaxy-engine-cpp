#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * pos;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_color;

void main()
{
    color = u_color;
}