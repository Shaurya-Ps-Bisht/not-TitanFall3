#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 7) in mat4 aInstanceMatrix;


out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float _Time;

void main()
{
    vec3 newPos = aPos;
    TexCoords = aTexCoords;    
    if(newPos.y > 0.1){
        newPos.x += sin(_Time) * 10.1;
        newPos.z += cos(_Time * 1.4) * 10.1;
    }
    FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * aInstanceMatrix *   vec4(newPos, 1.0);
}