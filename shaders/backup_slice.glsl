#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in float sliceIdx;

out vec3 VertexNormals;
out vec3 TextureSpacePosition;

uniform float rotX;

uniform mat4 rotation;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 FragPosInWorld  = vec3(model * vec4(vertexPosition + vec3(0.0f, 0.0f, sliceIdx), 1.0f));
    
    TextureSpacePosition = vec3(rotation * 1.25 * vec4(vec3(-0.5f) + vertexPosition + vec3(0.0f, 0.0f, sliceIdx), 1.0f)) + vec3(0.5f);
    gl_Position   = projection * view * vec4(FragPosInWorld, 1.0f);
    VertexNormals = vertexNormal;
}



=====================================

#version 330 core

out vec4 FragColor;
in  vec3 VertexNormals;
in  vec3 TextureSpacePosition;

uniform sampler3D TextureData;

void main()
{
    vec4 result = texture(TextureData, TextureSpacePosition);
    FragColor   = result;
}
