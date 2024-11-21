#version 330 core
out vec4 FragColor;

in vec3 FragPos;

void main()
{
    vec3 green = vec3(0.33, 0.66, 0.25);
    green = green * (FragPos.y + 0.2) + 0.1;
    FragColor = vec4(green, 1.0);
}