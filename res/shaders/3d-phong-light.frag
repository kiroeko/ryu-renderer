#version 460 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 lightWorldPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = vNormal;
    vec3 lightDir = normalize(lightWorldPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f);
}