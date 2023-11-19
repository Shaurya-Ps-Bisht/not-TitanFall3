#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 lightColor;

void main()
{    
    // FragColor = texture(skybox, TexCoords);
    FragColor = texture(skybox, TexCoords) * vec4(lightColor,1);
}