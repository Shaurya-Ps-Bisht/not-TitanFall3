#version 410 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform vec4 color;

void main()
{             
    FragColor = color;
}