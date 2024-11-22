#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    vec3 pos = aPos;
    float animStrength = 3.0;
    float sine = time;
    float sine2 = time * 1.7;
    sine = sin(sine);
    sine2 = sin(sine2);

     mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));

    pos.x += (sine + sine2) * 0.7 * (animStrength / 10.0) * pos.z;
    pos.y += (sine + sine2) * 0.3 * (animStrength / 10.0) * pos.z;

    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * aInstanceMatrix * vec4(pos, 1.0);
}  