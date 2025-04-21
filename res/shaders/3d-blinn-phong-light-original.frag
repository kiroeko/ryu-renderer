#version 460 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform vec3 lightWorldPos;
uniform vec3 cameraWorldPos;

out vec4 FragColor;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = vNormal;
    vec3 lightDir = normalize(lightWorldPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(cameraWorldPos - vWorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}