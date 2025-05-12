#version 460 core

in vec2 TexCoord;

uniform sampler2D mainTexture;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(mainTexture, TexCoord);
    if (texColor.a < 0.00001)
        discard;
    FragColor = texColor;
}