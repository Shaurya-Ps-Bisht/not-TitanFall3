#version 410 core
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
uniform sampler2DArray shadowMap;

uniform mat4 view;
uniform float farPlane;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal);

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light, float farPlane);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm, viewDir);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;

    vec4 mainTex = texture(texture_diffuse1, TexCoords);
    float shadow = ShadowCalculation(FragPos, dirLight, farPlane);
    
    vec3 final = vec3( (1-shadow) * diffuse  + ambient);

    for(int i = 0; i < 3; i++)
        final += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    final *= mainTex.xyz;

    FragColor = vec4(final,1);
    // FragColor = vec4(vec3(shadow), 1.0);;
    // FragColor = vec4(shadow,shadow,shadow,1);
    // FragColor = vec4(FragPosLightSpace.b,FragPosLightSpace.b,FragPosLightSpace.b,1);
    // FragColor = FragPosLightSpace;
    // FragColor = FragPosLightSpace;


}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0) ;
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // // combine results
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // return (ambient + diffuse + specular);
    return vec3( (diff +spec * 0.5) * dirLight.color);
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

    float intensity = 1.00;
    return (ambient + diffuse + specular) * intensity;
}

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light, float farPlane)
{
    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    float bias = max(0.05 * (1.0 - dot(normal, light.direction)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}


// float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal)
// {
//     vec3 lightDir = normalize(-light.direction);
//     // perform perspective divide
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//     // transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;
//     // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//     float closestDepth = texture(shadowMap, projCoords.xy).r; 
//     // get depth of current fragment from light's perspective
//     float currentDepth = projCoords.z;
//     // check whether current frag pos is in shadow
//     float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//     for(int x = -1; x <= 1; ++x)
//     {
//         for(int y = -1; y <= 1; ++y)
//         {
//             float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
//         }    
//     }
//     shadow /= 9.0;
  

//     if(projCoords.z > 1.0)
//     shadow = 0.0;

//     return shadow;
// }