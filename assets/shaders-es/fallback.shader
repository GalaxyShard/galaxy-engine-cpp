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

void main()
{
    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}