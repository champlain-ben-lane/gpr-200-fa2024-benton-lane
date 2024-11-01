#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int shininessStrength;

void main()
{
    vec3 ambient = ambientStrength * vec3(1.0); // assuming a bright white ambient light
    
    // Texture coords
    vec4 texInfo = texture(texture1, TexCoords);
  	
    // diffuse 
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininessStrength); // 32 is the shininess param
    vec3 specular = specularStrength * spec * lightColor; 
    
    vec4 lightingInfo = vec4((ambient + diffuse + specular), 1.0);
    
    vec4 result = lightingInfo * texInfo;
    
    FragColor = result;
} 