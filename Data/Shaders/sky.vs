#version 330 core

layout (location = 0) in vec3 InPosition;
out vec3 SampleDir;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	gl_Position = vec4(InPosition, 1.0f) * View * Projection;

	SampleDir = InPosition;
}