#version 410 core
struct DirLight {
    vec3 direction;
    vec3 color;
};

in float Height;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;


uniform sampler2D uHeightMap;
uniform sampler2D sandTex;
uniform sampler2DArray shadowMap;




uniform float farPlane;
uniform vec2 uTexelSize;
uniform vec3 viewPos;
uniform DirLight dirLight;

uniform mat4 view;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
// float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal);

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light, vec3 normal);



void main()
{    
    float HEIGHT_SCALE = 32.0;
    float left  = texture(uHeightMap, TexCoords + vec2(-uTexelSize.x, 0.0)).r * 100.0f - 16.0f;
    float right = texture(uHeightMap, TexCoords + vec2( uTexelSize.x, 0.0)).r * 100.0f - 16.0f;
    float up    = texture(uHeightMap, TexCoords + vec2(0.0,  uTexelSize.y)).r * 100.0f - 16.0f;
    float down  = texture(uHeightMap, TexCoords + vec2(0.0, -uTexelSize.y)).r * 100.0f - 16.0f;
    vec3 normal = normalize(vec3(left - right, 2.0, down - up));



    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm, viewDir);
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * dirLight.color;


    float h = (Height + 16)/64.0f;
    vec3 yellowColor = vec3(0.9, 0.85, 0.7);
    vec3 rockGreyColor = vec3(0.5, 0.5, 0.5);
    vec4 sand = texture(sandTex, TexCoords * 100.0);
    float shadow = ShadowCalculation(FragPos, dirLight, normal);

    vec3 color = mix(sand.xyz, rockGreyColor, clamp(h * 2.25, 0.0, 1.0)) * ((1 - shadow)*diffuse + ambient);
color = pow(color, vec3(1.0/2.2)); 
    FragColor = vec4(color, 1.0) ;

    


    // FragColor = vec4(normal, 1.0);
    // FragColor = texture(uHeightMap, TexCoords + vec2(0.0, -uTexelSize))* 64.0 - 16.0;
    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);

}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    // // combine results
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // return (ambient + diffuse + specular);
    return vec3(diff * dirLight.color +spec);
}

float ShadowCalculation(vec3 fragPosWorldSpace, DirLight light, vec3 normal)
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
    vec3 normal1 = normalize(normal);
    float bias = max(0.05 * (1.0 - dot(normal1, lightDir)), 0.005);
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

//     float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

//     if(projCoords.z > 1.0)
//         shadow = 0.0;

//     return shadow;
// }