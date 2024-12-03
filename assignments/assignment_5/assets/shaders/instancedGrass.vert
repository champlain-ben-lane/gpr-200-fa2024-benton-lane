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
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));

    vec3 pos = aPos;
    float animStrength = 3.0;
    float sine = time + (FragPos.x + FragPos.z) * 1;
    float sine2 = time * 1.7 + (FragPos.x + FragPos.z) * 1;
    float cossine = time * 2.45 + (FragPos.x) * 1;
    sine = sin(sine);
    sine2 = sin(sine2) / 1.5 - 0.3;
    cossine = cos(cossine) - 0.5;

     mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));

    FragPos.x += (sine + sine2 + cossine) * 0.7 * (animStrength / 10.0) * FragPos.y;
    FragPos.z += (sine + sine2 + cossine) * 0.3 * (animStrength / 10.0) * FragPos.y;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}  