#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;

void main()
{
    vec4 worldPos = model * vec4(pos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = projection * view * worldPos;
}