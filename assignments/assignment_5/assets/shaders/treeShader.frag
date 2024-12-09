//made by Olivia

#version 330 core

out vec4 FragColor;

in vec2 FragPos;

uniform sampler2D treeTex;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

void main()
{
	FragColor = texture(treeTex, FragPos);
}