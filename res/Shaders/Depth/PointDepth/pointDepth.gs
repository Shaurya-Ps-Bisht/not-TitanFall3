#version 430 core

#define NR_POINT_LIGHTS 16


layout(triangles) in;
layout(triangle_strip, max_vertices = 18 * NR_POINT_LIGHTS) out;

uniform mat4 shadowMatrices[6 * NR_POINT_LIGHTS];
out vec4 FragPos;

void main()
{          
	for(int lightNum = 0; lightNum < NR_POINT_LIGHTS; lightNum++)
	{
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