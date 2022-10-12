#version 330 core

in  vec2 TextureCoords;
out vec4 FragmentColor;

uniform sampler3D texture3d;
uniform mat3  uvPermutation;
uniform float sliceOffset;

void main()
{    
    vec3  sampleCoord = uvPermutation * vec3(TextureCoords.x, TextureCoords.y, sliceOffset);

    FragmentColor = vec4(texture(texture3d, sampleCoord).rgb, 1.0f);
}