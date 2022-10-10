#version 330 core

in  vec3 Texture3DCoords;
out vec4 FragmentColor;

uniform sampler3D texture3d;
uniform float sliceOffset;

void main()
{
	vec4 color = texture(texture3d, Texture3DCoords + vec3(0.0, sliceOffset, 0));
	FragmentColor = color;
}
