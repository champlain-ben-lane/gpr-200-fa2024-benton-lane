#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
out vec4 Color;
uniform float uTime;
void main()
{
	Color = aColor;
	vec3 pos = aPos;
	pos.y += sin(uTime * 5.0 + pos.x) / 4.0; // This is the line that makes the triangle bounce up and down
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}