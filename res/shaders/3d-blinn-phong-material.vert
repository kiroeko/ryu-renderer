#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 viewNormalMatrix;

out vec3 vViewPos;
out vec3 vViewNormal;
out vec2 vTexCoords;

void main()
{
    vec4 viewPos = view * model * vec4(pos, 1.0);
    vViewPos = viewPos.xyz;
    vViewNormal = normalize(viewNormalMatrix * normal);
    vTexCoords = texCoords;
    gl_Position = projection * viewPos;
}