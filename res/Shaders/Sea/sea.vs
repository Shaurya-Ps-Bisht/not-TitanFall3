#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform float _Time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 GerstnerWave(vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal) {
    float steepness = wave.z;
    float wavelength = wave.w;
    float k = 2.0 * 3.14159 / wavelength;
    float c = sqrt(9.8 / k);
    vec2 d = normalize(wave.xy);
    float f = k * (dot(d, p.xz) - c * _Time);
    float a = steepness / k;

    tangent += vec3(
        -d.x * d.x * (steepness * sin(f)),
        d.x * (steepness * cos(f)),
        -d.x * d.y * (steepness * sin(f))
    );

    binormal += vec3(
        -d.x * d.y * (steepness * sin(f)),
        d.y * (steepness * cos(f)),
        -d.y * d.y * (steepness * sin(f))
    );

    return vec3(
        a * cos(f),
        a * sin(f),
        0.0
    );
}

void main() {

    vec4 _WaveA  = vec4(1,0,0.05,1.25);
    vec4 _WaveB  = vec4(0,1,0.025,2.5);
    vec4 _WaveC  = vec4(1,1,0.015,1.25);

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    vec3 tangent = vec3(0);
    vec3 binormal = vec3(0);
    vec3 p = aPos;
    p += GerstnerWave(_WaveA, aPos, tangent, binormal);
    p += GerstnerWave(_WaveB, aPos, tangent, binormal);
    p += GerstnerWave(_WaveC, aPos, tangent, binormal);


    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(p, 1.0);
}
