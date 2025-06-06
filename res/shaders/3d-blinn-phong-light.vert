#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 viewNormalMatrix;
uniform vec3 lightWorldPos;

out vec3 vViewPos;
out vec3 vViewNormal;
out vec3 vLightViewPos;

void main()
{
    vec4 viewPos = view * model * vec4(pos, 1.0);
    vViewPos = viewPos.xyz;
    vViewNormal = normalize(viewNormalMatrix * normal);
    vLightViewPos = vec3(view * vec4(lightWorldPos, 1.0));
    gl_Position = projection * viewPos;
}