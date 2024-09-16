#version 330 core
out vec4 FragColor;
in vec4 Color;
uniform float uTime = 1.0;
uniform vec4 uColor = vec4(1.0);
void main()
{
	FragColor = Color * (sin(uTime) * 0.5 + 0.5);// This is the line that makes the colors go weeeee
}