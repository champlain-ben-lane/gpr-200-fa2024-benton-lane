//made by Olivia

#version 330 core

out vec4 FragColor;

in vec2 FragPos;

uniform sampler2D treeTex;

void main()
{
	FragColor = texture(treeTex, FragPos);
}