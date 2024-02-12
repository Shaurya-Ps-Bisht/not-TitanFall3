#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;
	
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
	
const int MAX_BONES = 200;
int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];
	
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
    mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
    BoneTransform += finalBonesMatrices[boneIds[1]] * weights[1];
    BoneTransform += finalBonesMatrices[boneIds[2]] * weights[2];
    BoneTransform += finalBonesMatrices[boneIds[3]] * weights[3];

    vec4 PosL = BoneTransform * vec4(pos, 1.0);
    gl_Position =   projection * view* model * PosL;
    vs_out.TexCoords = tex;
    vec4 NormalL = BoneTransform * vec4(norm, 0.0);
    vs_out.Normal = (model * NormalL).xyz;
    vs_out.FragPos = (model * PosL).xyz;

}