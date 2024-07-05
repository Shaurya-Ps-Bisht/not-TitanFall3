#version 420                                                        
layout (triangles) in;                                               
layout (triangle_strip, max_vertices = 36) out;                     
uniform mat3 cubemapCaptureTransforms[6];                                        
                                                                              
in VS_OUT                                                           
{                                                                   
    vec3 position;                                                   
} gs_in[];                                                        
                                                                              
out GS_OUT                                                         
{                                                                   
    vec3 position;                                                    
} gs_out;                                                          
                                                                              
void main()                                                          
{                                                                  
    for (int i = 0; i < 6; ++i)                                     
    {                                                                
        gl_Layer = i;                                              
        gl_Position = vec4(gs_in[0].position, 1.0f);                
        gs_out.position = cubemapCaptureTransforms[i] * vec3(gs_in[0].position.xy, 1.0f);      
        EmitVertex();                                               
                                                                              
        gl_Layer = i;                                                
        gl_Position = vec4(gs_in[1].position, 1.0f);                 
        gs_out.position = cubemapCaptureTransforms[i] * vec3(gs_in[1].position.xy, 1.0f);        
        EmitVertex();                                              
                                                                              
        gl_Layer = i;                                                
        gl_Position = vec4(gs_in[2].position, 1.0f);                
        gs_out.position = cubemapCaptureTransforms[i] * vec3(gs_in[2].position.xy, 1.0f);        
        EmitVertex();                                                
                                                                              
        EndPrimitive();                                              
    }                                                               
}     

