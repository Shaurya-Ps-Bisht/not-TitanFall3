#version 330 core

#define NR_POINT_LIGHTS 4


in vec4 FragPos;
flat in int lightIndex;


uniform vec3 lightPos[NR_POINT_LIGHTS];
uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos[lightIndex]);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  
