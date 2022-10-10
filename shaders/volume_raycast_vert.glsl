#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 rotation;
uniform mat4 projection;

out vec3 PosInWorld;

void main()
{
	PosInWorld  = vec3(rotation * vec4(vertexPosition, 1.0f));
	gl_Position = projection * view * rotation * vec4(vertexPosition, 1.0f);
}