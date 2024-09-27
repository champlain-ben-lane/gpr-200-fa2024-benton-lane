#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float uTime;

void main()
{
	vec2 aTexCoord = TexCoord;
	aTexCoord.x += uTime / 2.0f; // Affects the sin wave squash
	aTexCoord.y += sin(aTexCoord.x + uTime) / 2.0f; // Up and down strength
	// linearly interpolate between both textures (80% wall, 20% turret)
	FragColor = mix(texture(texture1, aTexCoord), texture(texture2, aTexCoord), 0.2);
}