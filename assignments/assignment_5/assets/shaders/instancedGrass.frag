//Made by Sam

#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;

void main()
{
    //grass color based on vert height(renders nicer) and distance from the light
    vec3 green = vec3(0.33, 0.66, 0.25);
    green = green * (FragPos.y + 0.2) + 0.1;
    float brightness = length(FragPos - lightPos);
    green = green/brightness;
    FragColor = vec4(green, 1.0);
}