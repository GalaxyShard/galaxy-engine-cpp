#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_mvp;
out vec2 v_texCoord;

void main()
{
    gl_Position = u_mvp * pos;
    v_texCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_tex;
in vec2 v_texCoord;

void main()
{
    color = texture(u_tex, v_texCoord);
}