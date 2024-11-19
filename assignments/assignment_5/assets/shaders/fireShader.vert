// billboarding tutorial here: https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

//made by Olivia

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraUp;

void main()
{
    // Calculate the rotation matrix
    mat4 rotation = rotate(mat4(1.0), cameraFront, cameraUp);

    // Apply the rotation to the billboard position
    vec3 billboardPos = (rotation * vec4(aPos, 1.0)).xyz;

    // Project the billboard position
    gl_Position = projection * view * vec4(billboardPos, 1.0);

    FragPos = billboardPos.xy;
  }