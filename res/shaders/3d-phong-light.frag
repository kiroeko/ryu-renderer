#version 460 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform float specularStrength;
uniform float shininess;
uniform vec3 lightWorldPos;
uniform vec3 lightColor;
uniform vec3 cameraWorldPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = vNormal;
    vec3 lightDir = normalize(lightWorldPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(cameraWorldPos - vWorldPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}