//made by Olivia

#version 330 core

layout(location = 0) in vec3 treeVertices;

out vec2 FragPos;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform vec3 CameraFront_worldspace;
uniform mat4 VP;
uniform vec3 BillboardPos;
uniform vec2 BillboardSize;

void main() {

    vec3 vertexPosition_worldspace = 
        BillboardPos
        + CameraRight_worldspace * treeVertices.x * BillboardSize.x
        + vec3(0,1,0) * treeVertices.y * BillboardSize.y;

    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
    FragPos = treeVertices.xy + vec2(0.5, 0.5);
}
