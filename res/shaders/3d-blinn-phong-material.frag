#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

in vec3 vViewPos;
in vec3 vViewNormal;
in vec3 vLightViewPos;

uniform vec3 lightColor;
uniform Material material;

out vec4 FragColor;

void main()
{
    vec3 ambient = lightColor * material.ambient;

    vec3 norm = vViewNormal;
    vec3 lightDir = normalize(vLightViewPos - vViewPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * lightColor;

    vec3 viewDir = normalize(-vViewPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}