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
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;

void main()
{    
    float distance = length(pointLights[3].position  - FragPos);
    float attenuation = 1.0 / (pointLights[3].constant + pointLights[3].linear * distance + pointLights[3].quadratic * (distance * distance));
    vec3 pointColor = pointLights[3].color * attenuation;    

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;

    FragColor = texture(texture_diffuse1, TexCoords) * (vec4(dirLight.color,1) );
    if(FragColor.a < 0.2) {
        discard;
    }
}