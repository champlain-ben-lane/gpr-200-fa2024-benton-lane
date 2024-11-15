// tutorial for fire texture here: https://blog.febucci.com/2019/05/fire-shader/
// converted tutorial from HLSL to OpenGL

#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in float deltaTime;

uniform sampler2D noiseTex;
uniform sampler2D gradientTex;

void main()
{
	float noiseValue = (noiseTex, TexCoord - deltaTime).x;
	float gradientValue = (gradientTex, TexCoord).x;
	float step1 = step(noiseValue, gradientValue);
	float step2 = step(noiseValue, gradientValue-0.2);
	float step3 = step(noiseValue, gradientValue-0.4);

	//FragColor = vec4(mix((1.0f, 1.0f, 0.0f), (1.0f, 0.0f, 0.0f), step1 - step2);
	//FragColor = vec4(mix(FragColor, (1.0f, 0.5f, 0.0f), step 2 - step3);
	FragColor = vec4(1.0f);
}