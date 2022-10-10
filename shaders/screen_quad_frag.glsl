#version 330 core


in  vec2 TextureCoords;
out vec4 FragmentColor;

uniform sampler2D textureData;

void main()
{
	FragmentColor = texture(textureData, TextureCoords);
	//FragmentColor = vec4(1.0f);
}