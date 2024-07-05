#version 430 core

#define NR_POINT_LIGHTS 4


layout(triangles) in;
layout(triangle_strip, max_vertices = 72) out; //18 * 4 (can only put the final thing here )

uniform mat4 shadowMatrices[6 * NR_POINT_LIGHTS];
out vec4 FragPos;
flat out int lightIndex;

void main()
{          
	for(int lightNum = 0; lightNum < NR_POINT_LIGHTS; lightNum++)
	{
		lightIndex = lightNum;
		for(int face = 0; face < 6; ++face)
		{
			gl_Layer = face + 6 * lightNum; 
			for(int i = 0; i < 3; ++i) 
			{
				FragPos = gl_in[i].gl_Position;
				gl_Position = shadowMatrices[face + 6 * lightNum] * FragPos;
				EmitVertex();
			}      
			EndPrimitive();
		}
	}

}  
