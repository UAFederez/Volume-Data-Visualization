#version 330 core

in  vec2 TextureCoords;
out vec4 FragmentColor;

uniform sampler3D texture3d;
uniform mat3  uvPermutation;
uniform float sliceOffset;
uniform float maxValue;
uniform float minValue;

void main()
{    
    vec3 sampleCoord = uvPermutation * vec3(TextureCoords.x, TextureCoords.y, sliceOffset);
    vec3 fragColor   = (texture(texture3d, sampleCoord).rgb - vec3(minValue)) / (vec3(maxValue - minValue));
    FragmentColor = vec4(fragColor, 1.0f);
}