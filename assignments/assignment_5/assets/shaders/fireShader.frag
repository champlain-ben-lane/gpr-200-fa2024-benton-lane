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
	// apply textures, roll noise
    float noiseValue = texture(noiseTex, TexCoord - deltaTime * 0.01).r;
    float gradientValue = texture(gradientTex, TexCoord).r;
    
    // create solid transitions between colors
    float step1 = step(noiseValue, gradientValue);
    float step2 = step(noiseValue, gradientValue - 0.2);
    float step3 = step(noiseValue, gradientValue - 0.4);
    
    // fire colors
    vec4 col = vec4(mix(vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), step1 - step2), step1);
    
    col.rgb = mix(.rgb,vec3(1.0f, 0.5f, 0.0f), step2 - step3);
    
    FragColor = col;
}