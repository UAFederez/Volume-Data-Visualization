#version 450 core

in  vec2 TextureCoords;
out vec4 FragmentColor;

uniform sampler2D quadTexture;

void main()
{
    FragmentColor = texture(quadTexture, TextureCoords);
}