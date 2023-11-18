#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirLight {
    vec3 direction;
    vec3 color;
};

uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;

vec3 CalcDirLight(DirLight light, vec3 normal);

void main()
{    
    vec3 norm = normalize(Normal);
    // vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = CalcDirLight(dirLight, norm);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * dirLight.color;

    vec3 lighFinal = diffuse + ambient;

    vec4 mainTex = texture(texture_diffuse1, TexCoords);

    vec4 final = mainTex * vec4(diffuse + ambient,1);
    

    FragColor = final;


}

vec3 CalcDirLight(DirLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // // combine results
    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // return (ambient + diffuse + specular);
    return vec3(diff,diff,diff);
}