#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}  