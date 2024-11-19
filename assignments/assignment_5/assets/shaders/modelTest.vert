#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBitan;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 tanLightPos;
out vec3 tanViewPos;
out vec3 tanFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos; 
uniform vec3 viewPos; 

void main()
{    
    FragPos = vec3(model * vec4(aPos, 1.0));

    //calcualtes normal matrix
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTan);
    vec3 N = normalize(normalMatrix * aNormal);    
    vec3 B = normalize(normalMatrix * aBitan);      
    //T = normalize(T - dot(T, N) * N);
    mat3 TBN = transpose(mat3(T, B, N));   

    //calculates lighting elements based on normal matrix. passed to frag shader
    tanLightPos = TBN * lightPos;
    tanViewPos = TBN * viewPos;
    tanFragPos = TBN * FragPos;

    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}