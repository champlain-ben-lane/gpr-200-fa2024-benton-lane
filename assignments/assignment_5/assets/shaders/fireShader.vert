// billboarding tutorial here: https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

//made by Olivia

#version 330 core

layout(location = 0) in vec3 fireVertices;

out vec2 FragPos;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform vec3 CameraFront_worldspace;
uniform mat4 VP;
uniform vec3 BillboardPos;
uniform vec2 BillboardSize;
uniform float timeElapsed;
uniform float flickerStrength;
uniform float windSpeed;

void main() {
    // Calculate a random offset for the vertex
    vec2 offset = vec2(
        sin((fireVertices.x + timeElapsed) * 20.0 * windSpeed) * flickerStrength * 0.015,
        sin((fireVertices.y + timeElapsed) * 20.0 * windSpeed) * flickerStrength * 0.015
    );

    // Apply the offset to the vertex position
    vec3 vertexPosition_worldspace = 
        BillboardPos
        + CameraRight_worldspace * (fireVertices.x + offset.x) * BillboardSize.x
        + vec3(0,1,0) * (fireVertices.y + offset.y) * BillboardSize.y;

    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
    FragPos = fireVertices.xy + vec2(0.5, 0.5);
}