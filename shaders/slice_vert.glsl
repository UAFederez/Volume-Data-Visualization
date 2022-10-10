#version 330 core
layout (location = 0) in vec3  vertexPosition;
layout (location = 1) in float sliceOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 faceScale;
uniform mat4 projection;

uniform float zMin;
uniform float zMax;

out float offset;
out vec4 VertPosition;

void main()
{
    vec4 vPos = vec4(vertexPosition.x, vertexPosition.y, 0.0f, 1.0f);
    vec4 rescaledAndShifted = (faceScale * model * vec4(vertexPosition, 1.0f)) + vec4(0.0f, 0.0f, zMin + sliceOffset * (zMax - zMin), 1.0f);
    
    gl_Position  = projection * view * rescaledAndShifted;
    VertPosition = vec4(vertexPosition, 1.0f);
    offset = sliceOffset;
}
