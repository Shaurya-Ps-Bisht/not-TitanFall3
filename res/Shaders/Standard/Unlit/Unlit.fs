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

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;



uniform vec3 viewPos;
uniform float farPlane;
uniform float pointShadowFar;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2DArray shadowMap;
uniform samplerCubeArray pointShadowMap;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);




uniform mat4 view;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index);

// float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal);

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light);
float PointShadowCalculation(PointLight light, vec3 fragPos, int index);

void main()
{    
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm, viewDir);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;

    vec4 mainTex = texture(texture_diffuse1, fs_in.TexCoords);
    float shadow = ShadowCalculation(fs_in.FragPos, dirLight);
    
    vec3 final = vec3( (1-shadow) * diffuse  + ambient);

    for(int i = 0; i <4; i++)
    {
        final += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, i);
    }

    final = mainTex.xyz;

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index)
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

    vec3 ambient = light.color * 0.1;
    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float intensity = 0.5;
    return (ambient + (diffuse + specular) * (1.0 - PointShadowCalculation(light, fs_in.FragPos, index))
    ) * intensity;
}

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light)
{
    // select cascade layer
    vec3 lightDir = light.direction;
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
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
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

float PointShadowCalculation(PointLight light, vec3 fragPos, int index)
{
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / pointShadowFar)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, vec4(fragToLight + gridSamplingDisk[i] * diskRadius, index)).r;
        closestDepth *= pointShadowFar;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
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