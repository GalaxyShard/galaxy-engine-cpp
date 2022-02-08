#shader vertex
#version 100

attribute vec4 pos;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_rotation;

varying vec3 v_position;
varying vec2 v_texCoord;
varying vec3 v_normal;
void main()
{
    gl_Position = u_mvp * pos;
    v_position = vec3(u_model * pos);
    v_texCoord = texCoord;
    v_normal = normalize(vec3(u_rotation * vec4(normal, 1.0)));
}
#shader fragment
#version 100
precision highp float;

uniform vec3 u_camPos;

uniform sampler2D u_tex;
uniform vec4 u_lightColor;
uniform vec3 u_lightPos;
uniform float ambient;

varying vec3 v_position;
varying vec2 v_texCoord;
varying vec3 v_normal;
void main()
{
    vec3 objToLight = normalize(u_lightPos - v_position);
    float diffuse = max(dot(objToLight, v_normal), 0.0);

    float specularLight = 0.5;
    vec3 objToView = normalize(u_camPos - v_position);
    vec3 reflectionDir = reflect(-objToLight, v_normal);
    float specular = pow(max(dot(objToView, reflectionDir), 0.0), 8.0) * specularLight;
    gl_FragColor = texture2D(u_tex, v_texCoord) * u_lightColor * (diffuse+ambient+specular);
}