#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirLight {
    vec3 direction;
    vec3 color;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
float CalculateFresnel(vec3 viewDir, vec3 normal, float F0);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm, viewDir) ;
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * dirLight.color;

    float fresnel = CalculateFresnel(viewDir, vec3(0.0, 1.0, 0.0), 0.04);

    // Adjusted colors and transparency for water
    vec3 waterColor = vec3(0.1, 0.5, 0.9) * dirLight.color;

    float transparency = 1.0 - dot(norm, viewDir);
    transparency = clamp(transparency, 0.0, 1.0);

    vec4 final = vec4(waterColor * (diffuse + ambient) * (1.0 - fresnel) + fresnel * waterColor, transparency);

    // final.xyz = pow(final.xyz, vec3(1.0/2.2)); 
    FragColor = final;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0) ;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5);
    return vec3(diff * dirLight.color + spec);
}

float CalculateFresnel(vec3 viewDir, vec3 normal, float F0)
{
    float cosTheta = max(dot(normal, viewDir), 0.0);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
