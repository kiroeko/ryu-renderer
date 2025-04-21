#version 460 core

in vec3 vViewPos;
in vec3 vViewNormal;
in vec3 vLightViewPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

out vec4 FragColor;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = vViewNormal;
    vec3 lightDir = normalize(vLightViewPos - vViewPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-vViewPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}