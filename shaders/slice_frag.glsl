#version 330 core

out vec4 FragColor;
in  vec4 VertPosition;
in  float offset;

uniform sampler3D texture3d;

uniform mat4 volumeRotation;
uniform mat4 model;
uniform mat4 faceScale;
uniform vec3 volumeCenter;

uniform float zMin;
uniform float zMax;

void main()
{
    vec4 vPos    = vec4(VertPosition.x, VertPosition.y, 0.0f, 1.0f);
    vec4 offs    = (faceScale * model * vPos) + vec4(0.0f, 0.0f, zMin + offset * (zMax - zMin), 0.0f);
    vec4 rotated = transpose(volumeRotation) * offs;
    vec4 shifted = rotated + vec4(volumeCenter.xyz, 1.0f);    // should be the volume center

    FragColor    = texture(texture3d, vec3(shifted)).rgbr;
    //FragColor    = vec4(1.0f);
}
