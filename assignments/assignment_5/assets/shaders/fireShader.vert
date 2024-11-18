// billboarding tutorial here: https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

#version 330 core

in vec3 aPos;
in vec2 aTexCoord;

out vec2 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform vec3 cameraUp;
uniform vec3 billboardSize;

void main()
{
    // Calculate the billboard position
    vec3 billboardPos = cameraPos + cameraUp * aPos.y * billboardSize.y;

    // Calculate the vertex position
    vec3 vertexPos = billboardPos + cameraUp * aPos.x * billboardSize.x;

    // Transform the vertex position
    gl_Position = projection * view * vec4(vertexPos, 1.0);

    FragPos = aTexCoord;
}