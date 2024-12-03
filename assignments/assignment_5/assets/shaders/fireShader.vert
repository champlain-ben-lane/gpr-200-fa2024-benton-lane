// billboarding tutorial here: https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

//made by Olivia

#version 330 core

layout(location = 0) in vec3 fireVertices;

out vec2 FragPos;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP;
uniform vec3 BillboardPos;
uniform vec2 BillboardSize;

void main() {

    vec3 vertexPosition_worldspace = 
        BillboardPos 
        + CameraRight_worldspace * fireVertices.x * BillboardSize.x
        + CameraUp_worldspace * fireVertices.y * BillboardSize.y;

    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
    FragPos = fireVertices.xy + vec2(0.5, 0.5);
}