#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture3;

void main()
{
	FragColor = texture(texture3, TexCoord); 
}