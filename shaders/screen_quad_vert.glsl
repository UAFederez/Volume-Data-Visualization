#version 450 core

layout (location = 0) in vec2 vertexPosition;
layout (location = 0) in vec2 textureCoords;

out vec2 TextureCoords;

void main()
{
    gl_Position   = vec4(vertexPosition, 0.0f, 1.0f);
    TextureCoords = textureCoords;
}