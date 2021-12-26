#shader vertex
#version 100

attribute vec4 pos;
attribute vec2 texCoord;

uniform mat4 u_mvp;
varying vec2 v_texCoord;

void main()
{
    gl_Position = u_mvp * pos;
    v_texCoord = texCoord;
}

#shader fragment
#version 100

uniform sampler2D u_tex;
varying highp vec2 v_texCoord;

void main()
{
    gl_FragColor = texture2D(u_tex, v_texCoord);
}