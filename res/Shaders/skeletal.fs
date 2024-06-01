#version 330 core
out vec4 FragColor;


struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;  
  
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm, viewDir);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;

    vec4 mainTex = texture(texture_diffuse1, TexCoords);
    vec3 final =  vec3(diffuse + ambient);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        final += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    final *= mainTex.xyz;

    // final = pow(final, vec3(1.0/2.2)); 

    FragColor = vec4(final,1);


}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0) ;
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5);
    // // combine results
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // return (ambient + diffuse + specular);
    return vec3(diff * dirLight.color +spec);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results

    vec3 ambient = light.color * 0.2;
    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float intensity = 0.25;
    return (ambient + diffuse + specular) * intensity;
}

