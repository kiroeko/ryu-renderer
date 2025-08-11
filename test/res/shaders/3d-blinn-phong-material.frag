#version 460 core

#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

struct DirectionalLight {
    vec3 color;
    vec3 viewDirection;
};

struct PointLight {
    vec3 color;
    vec3 viewPos;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
};

struct SpotLight {
    vec3 color;
    vec3 viewPos;
    vec3 viewDirection;
    float innerCutOffCos;
    float outerCutOffCos;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
}; 

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

in vec3 vViewPos;
in vec3 vViewNormal;
in vec2 vTexCoords;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform int activePointLightCount = 0;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activeSpotLightCount = 0;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform bool hasDiffuse = false;
uniform bool hasSpecular = false;
uniform bool hasEmission = false;

out vec4 FragColor;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewPos, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewPos, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture);

void main()
{
    vec3 viewPos = vViewPos;
    vec3 normal = vViewNormal;
    vec3 viewDir = normalize(-vViewPos);

    vec3 diffuseTexture = vec3(0.0);
    if (hasDiffuse)
        diffuseTexture = vec3(texture(material.diffuse, vTexCoords));
    vec3 specularTexture = vec3(0.0);
    if (hasSpecular)
        specularTexture = vec3(texture(material.specular, vTexCoords));
    vec3 emissionTexture = vec3(0.0);
    if (hasEmission)
        emissionTexture = vec3(texture(material.emission, vTexCoords));

    vec3 result = vec3(0.0);
    
    // Directional Light
    if (length(directionalLight.color) > 0.0)
    {
        result += CalcDirectionalLight(directionalLight, normal, viewDir, diffuseTexture, specularTexture);
    }
    
    // Point Lights
    for (int i = 0; i < activePointLightCount; ++i)
        result += CalcPointLight(pointLights[i], normal, viewPos, viewDir, diffuseTexture, specularTexture);

    // Spot Lights
    for (int i = 0; i < activeSpotLightCount; ++i)
        result += CalcSpotLight(spotLights[i], normal, viewPos, viewDir, diffuseTexture, specularTexture);

    // Emission
    result += emissionTexture;

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 lightDir = normalize(-light.viewDirection);

    vec3 ambient = material.ambient * diffuseTexture;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * diffuseTexture;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.color * spec * specularTexture;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewPos, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 posToLight = light.viewPos - viewPos;
    vec3 lightDir = normalize(posToLight);

    float dis = length(posToLight);
    float attenuation = 1.0 / (light.attenuationConstant + light.attenuationLinear * dis + light.attenuationQuadratic * (dis * dis));

    vec3 ambient = attenuation * material.ambient * diffuseTexture;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = attenuation * light.color * diff * diffuseTexture;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = attenuation * light.color * spec * specularTexture;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewPos, vec3 viewDir, vec3 diffuseTexture, vec3 specularTexture)
{
    vec3 posToLight = light.viewPos - viewPos;
    vec3 lightDir = normalize(posToLight);

    float dis = length(posToLight);
    float attenuation = 1.0 / (light.attenuationConstant + light.attenuationLinear * dis + light.attenuationQuadratic * (dis * dis));

    float theta = dot(lightDir, normalize(-light.viewDirection));

    vec3 ambient = attenuation * material.ambient * diffuseTexture;
    if (theta <= light.outerCutOffCos) 
    {
        return ambient;
    }
    else
    {
        float epsilon = light.innerCutOffCos - light.outerCutOffCos;
        float intensity = clamp((theta - light.outerCutOffCos) / epsilon, 0.0, 1.0);    

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = attenuation * intensity * light.color * diff * diffuseTexture;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = attenuation * intensity * light.color * spec * specularTexture;

        return ambient + diffuse + specular;
    }
}