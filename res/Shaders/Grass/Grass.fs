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

uniform mat4 view;
uniform vec3 viewPos;
uniform float farPlane;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;
uniform sampler2DArray shadowMap;


layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light);


void main()
{    
    // float distance = length(pointLights[3].position  - FragPos);
    // float attenuation = 1.0 / (pointLights[3].constant + pointLights[3].linear * distance + pointLights[3].quadratic * (distance * distance));
    // vec3 pointColor = pointLights[3].color * attenuation;    

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;
    float shadow = ShadowCalculation(FragPos, dirLight);

    vec4 color = texture(texture_diffuse1, TexCoords) * (vec4(ambient,1) + vec4(dirLight.color,1) * (1 - shadow));

    if(color.a < 0.2) {
        discard;
    }

    
    // color.xyz = pow(color.xyz, vec3(1.0/2.2)); 
    FragColor = color;

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
    vec3 normal = normalize(Normal);
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
