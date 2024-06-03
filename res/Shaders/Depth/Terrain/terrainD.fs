#version 410 core
struct DirLight {
    vec3 direction;
    vec3 color;
};

in float Height;
in vec2 TexCoords;
in vec3 FragPos;
layout (location = 0) out vec4 FragColor;


uniform sampler2D uHeightMap;
uniform sampler2D sandTex;
uniform vec2 uTexelSize;
uniform DirLight dirLight;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


void main()
{    
    // float HEIGHT_SCALE = 32.0;
    // float left  = texture(uHeightMap, TexCoords + vec2(-uTexelSize.x, 0.0)).r * 100.0f - 16.0f;
    // float right = texture(uHeightMap, TexCoords + vec2( uTexelSize.x, 0.0)).r * 100.0f - 16.0f;
    // float up    = texture(uHeightMap, TexCoords + vec2(0.0,  uTexelSize.y)).r * 100.0f - 16.0f;
    // float down  = texture(uHeightMap, TexCoords + vec2(0.0, -uTexelSize.y)).r * 100.0f - 16.0f;
    // vec3 normal = normalize(vec3(down - up, 2.0, left - right));

    // vec3 norm = normalize(normal);
    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 diffuse = CalcDirLight(dirLight, norm, viewDir);
    // float ambientStrength = 0.2;
    // vec3 ambient = ambientStrength * dirLight.color;


    // float h = (Height + 16)/64.0f;
    // vec3 yellowColor = vec3(0.9, 0.85, 0.7);
    // vec3 rockGreyColor = vec3(0.5, 0.5, 0.5);
    // vec4 sand = texture(sandTex, TexCoords * 100.0);
    // vec3 color = mix(sand.xyz, rockGreyColor, clamp(h * 2.25, 0.0, 1.0)) * (diffuse + ambient);

    // FragColor = vec4(color, 1.0) ;

    


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
