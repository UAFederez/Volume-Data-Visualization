#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 tex3dCoords;

out vec3 Texture3DCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 WorldFragPos = model * vec4(vertexPosition, 1.0f);
	gl_Position = projection * view * WorldFragPos;
	Texture3DCoords = tex3dCoords;
}
