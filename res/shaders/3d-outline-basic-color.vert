#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outline;

void main()
{
    vec3 pos = aPos + outline * normalize(aNormal);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}