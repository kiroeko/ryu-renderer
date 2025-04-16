#version 460 core
in vec2 TexCoord;

uniform sampler2D diffuseTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(diffuseTexture, TexCoord);
}