#shader vertex
#version 100

attribute vec4 pos;
uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * pos;
}

#shader fragment
#version 100

uniform lowp vec4 u_color;
void main()
{
    gl_FragColor = u_color;
}