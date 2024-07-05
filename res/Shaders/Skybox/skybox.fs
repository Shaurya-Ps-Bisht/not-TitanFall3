#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 lightColor;

void main()
{    
    // FragColor = texture(skybox, TexCoords);
    vec3 envColor = texture(skybox, TexCoords).rgb * lightColor;
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}
