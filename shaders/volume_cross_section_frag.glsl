#version 330 core

in  vec2 TextureCoords;
out vec4 FragmentColor;

uniform sampler3D texture3d;
uniform sampler2D textureColor;
uniform mat3  uvPermutation;
uniform float sliceOffset;
uniform float maxValue;
uniform float minValue;

void main()
{    
    vec3 sampleCoord   = uvPermutation * vec3(TextureCoords.x, TextureCoords.y, sliceOffset);
    float textureSample = (texture(texture3d, sampleCoord).r - minValue) / (maxValue - minValue);
    FragmentColor = texture(textureColor, vec2(textureSample, 0.0f));
}