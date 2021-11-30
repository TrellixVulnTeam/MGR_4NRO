#version 330 core
layout(location = 0) 
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

void main()
{
    vec3 lightColor = vec3(1.0f,1.0f,1.0f);
    vec3 objectColor = vec3(vs_out.Color);
    vec3 lightPos = vec3(10.0f,10.0f,10.0f);
    vec3 viewPos = vec3(0.0f,0.0f,10.0f);
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(vs_out.Normal);
    vec3 lightDir = normalize(lightPos - vs_out.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - vs_out.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}