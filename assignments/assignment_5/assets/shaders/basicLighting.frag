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

void main()
{
    // Texture coords
    vec3 color = texture(texture1, TexCoords).rgb;

    // ambient
    vec3 ambient = 0.05 * color;
  	
    // diffuse 
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
    vec3 specular = vec3(0.3) * spec; // assuming a bright white lightColor
    FragColor = vec4(ambient + diffuse + specular, 1.0);
} 