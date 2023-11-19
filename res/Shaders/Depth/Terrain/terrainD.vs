#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
out vec3 FragPos;

void main()
{
    FragPos = vec3(vec4(aPos, 1.0));
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTex;
}