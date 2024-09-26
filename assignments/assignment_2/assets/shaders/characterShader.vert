#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float uTime;

void main()
{
    vec3 pos = aPos;
	pos.y += sin(uTime * 5.0) / 4.0; // This is the line that makes the triangle bounce up and down
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}