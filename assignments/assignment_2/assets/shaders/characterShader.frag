#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture3;

void main()
{
	vec4 texColor = texture(texture3, TexCoord);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}