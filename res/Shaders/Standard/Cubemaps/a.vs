#version 420
in vec3 POSITION;
                                                                                
out VS_OUT                                                            
{                                                                    
   vec3 position;                                                     
} vs_out;                                                            
                                                                                
void main()                                                           
{                                                                   
    vs_out.position = POSITION;  
}

