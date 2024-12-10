// Made by Ben

#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 tanLightPos;
in vec3 tanViewPos;
in vec3 tanFragPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform sampler2D normal1;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int shininessStrength;

void main()
{
    vec3 ambient = ambientStrength * vec3(1.0); // assuming a bright white ambient light
    
    // Texture coords
    vec4 texInfo = texture(texture1, TexCoords);
    vec3 norm = texture(normal1, TexCoords).rgb; // get normal vector from texture
  	
    norm = normalize(norm * 2.0 - 1.0);
    
    // diffuse 
    vec3 lightDir = normalize(tanLightPos - tanFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    
    // specular
    float specularCheck = dot(norm, lightDir); //prevents light bounces without diffuse lighting
    float spec = 0.0;
    if (specularCheck > 0.0)
    {
        vec3 viewDir =  normalize(tanViewPos - tanFragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), shininessStrength); // 32 is the shininess param
    }
   
    vec3 specular = specularStrength * spec * lightColor; 
    
    vec4 lightingInfo = vec4((ambient + diffuse + specular), 1.0);
    
    vec4 result = lightingInfo * texInfo;
    //vec4 result = vec4(norm, 1.0);

    FragColor = result;
} 