#version 460 core

struct Material {
    float ambientStrength;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

in vec3 vViewPos;
in vec3 vViewNormal;
in vec3 vLightViewPos;
in vec2 vTexCoords;

uniform vec3 lightColor;
uniform Material material;

out vec4 FragColor;

void main()
{
    vec3 diffuseTexture = vec3(texture(material.diffuse, vTexCoords));
    vec3 ambient = material.ambientStrength * diffuseTexture;

    vec3 norm = vViewNormal;
    vec3 lightDir = normalize(vLightViewPos - vViewPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * diffuseTexture;

    vec3 viewDir = normalize(-vViewPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = lightColor * spec * vec3(texture(material.specular, vTexCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}