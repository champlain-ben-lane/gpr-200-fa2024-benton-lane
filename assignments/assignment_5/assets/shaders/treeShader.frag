//made by Olivia

#version 330 core

out vec4 FragColor;

in vec2 FragPos;

uniform sampler2D treeTex;

void main()
{
    vec4 texColor = texture(treeTex, FragPos);
    
    if (texColor.a > 0.0) {
        FragColor = vec4(vec3(0.02, 0.03, 0.02), texColor.a);
    } else {
        FragColor = texColor;
    }
}