// tutorial for fire texture here: https://blog.febucci.com/2019/05/fire-shader/
// converted tutorial from HLSL to OpenGL

//made by Olivia

#version 330 core

out vec4 FragColor;

in vec2 FragPos;

uniform float timeElapsed;
uniform sampler2D noiseTex;
uniform sampler2D gradientTex;

void main()
{
	 // Apply textures, roll noise
    float noiseValue = texture(noiseTex, vec2(FragPos.x, FragPos.y - timeElapsed)).r;
    float gradientValue = texture(gradientTex, FragPos).r;
    
    // Create solid transitions between colors
    float step1 = step(noiseValue, gradientValue);
    float step2 = step(noiseValue, gradientValue - 0.3);
    float step3 = step(noiseValue, gradientValue - 0.5);
    float step4 = step(noiseValue, gradientValue - 0.45);
    
    // Fire colors
    vec4 col = vec4(mix(vec3(0.5f, 0.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), step1 - step2), step1);
    
    // Yellow to orange
    col.rgb = mix(col.rgb, vec3(1.0f, 0.5f, 0.0f), step2 - step3);
    
    // Top layer
    col.rgb = mix(col.rgb, vec3(0.5f, 0.0f, 0.5f), step3 - step4);
    
    // make transparent
    if (col.r > 0.4 && col.r < 0.6 && col.g < 0.1 && col.b > 0.4 && col.b < 0.6)
        col.a = 0.0;

    // Feather edges
    float edgeDistance = length(FragPos - vec2(0.5, 0.5));
    col.a *= exp(-edgeDistance * 2.0) * pow(smoothstep(0.55, 1.0, 1.0 - edgeDistance), 0.6);
    
    FragColor = col;
}