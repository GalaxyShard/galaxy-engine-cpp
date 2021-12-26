#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
uniform vec3 u_pos;
uniform vec3 u_scale;

//uniform vec4 urotation;
//out vec2 v_texCoord;
//uniform vec3 u_aspect;
void main()
{

    gl_Position = vec4(pos * u_scale + u_pos, 1);

    //gl_Position = vec4(pos * uscale + upos, 1);
    //gl_Position = pos;
    //vec3 rotated = vec3(
    //    pos.x * cos(urotation.z) + pos.y * sin(urotation.z),
    //    -pos.x * sin(urotation.z) + pos.y * cos(urotation.z),
    //    pos.z);
    //gl_Position = vec4(rotated * uscale + upos, 1.0);
    //v_texCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

//uniform sampler2D u_texture;
//in vec2 v_texCoord;
void main()
{
    //color = texture(u_texture, v_texCoord);
    color = vec4(
        0,
        (gl_PointCoord.x + 1) / 2 + 0.1,
        1,
        1);
}