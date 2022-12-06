#version 450 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 rotation;
uniform mat4 projection;
uniform vec3 origin;

out vec3 PosInWorld;
out vec3 Origin;

void main()
{
	Origin	    = vec3(rotation * vec4(origin, 1.0f));
	PosInWorld  = vec3(rotation * model * vec4(vertexPosition, 1.0f));
	gl_Position = projection * view * rotation * model * vec4(vertexPosition, 1.0f);
}