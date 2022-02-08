#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_rotation;

out vec3 v_position;
out vec3 v_normal;
void main()
{
    gl_Position = u_mvp * pos;
    v_position = vec3(u_model * pos);
    v_normal = normalize(vec3(u_rotation * vec4(normal, 1)));
}
#shader fragment
#version 330 core

out vec4 color;
uniform vec3 u_camPos;

uniform vec4 u_color;
uniform vec4 u_lightColor;
uniform vec3 u_lightPos;
uniform float ambient;

in vec3 v_position;
in vec3 v_normal;
void main()
{
    vec3 objToLight = normalize(u_lightPos - v_position);
    float diffuse = max(dot(objToLight, v_normal), 0);

    float specularLight = 0.5;
    vec3 objToView = normalize(u_camPos - v_position);
    vec3 reflectionDir = reflect(-objToLight, v_normal);
    float specular = pow(max(dot(objToView, reflectionDir), 0), 8) * specularLight;
    color = u_color * u_lightColor * (diffuse+ambient+specular);
}