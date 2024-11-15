// billboarding tutorial here: https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

#version 330 core

in vec2 TexCoord;
in vec3 cameraSide;
in vec3 cameraUp;

out vec3 FragPos;

uniform vec3 fireWorldSpace;
uniform vec3 billboardSize;

void main()
{
	//FragPos = vec3(fireWorldSpace, cameraSide.x * billboardSize.x, cameraUp.y * billboardSize.y);
	FragPos = vec3(1.0f);
}