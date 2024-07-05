#version 420                                             
in GS_OUT                                              
{                                                    
    vec3 position;                                       
} fs_in;                                                
                                                                  
out vec4 color;                                     
                                                                  
void main()                                            
{                                                      
    color = vec4(fs_in.position, 1.0f);                
}          

