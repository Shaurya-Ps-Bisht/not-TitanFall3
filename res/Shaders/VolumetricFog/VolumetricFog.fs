#version 330 core
out vec4 FragColor;


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform float _Scale;
uniform float _StepScale;
uniform float _Steps;
uniform float _MinHeight;
uniform float _MaxHeight;
uniform float _FadeDistance;
uniform vec4 _SunDir;

float random(vec3 value, vec3 dotDir){
    vec3 smallV = sin(value);
    float random = dot(smallV, dotDir);
    random = fract(sin(random) * 123574.43212);
    return random;
}

vec3 random3d(vec3 value){
    return vec3(
        random(value, vec3(12.898, 68.54, 37.54)),
        random(value, vec3(22.898, 18.54, 99.54)),
        random(value, vec3(1.898, 8.54, 45.54))
    );
}

float noise3d(vec3 value){
    value *= _Scale;
    vec3 interp = fract(value);
    interp = smoothstep(0.0, 1.0, interp);

    vec3 ZValues[2];
    for(int z = 0; z <= 1; z++){
        vec3 YValues[2];

        for(int y = 0; y <= 1; y++){
            vec3 XValues[2];
            for(int x = 0; x <= 1; x++){
                vec3 cell = floor(value) + vec3(x, y, z);
                XValues[x] = random3d(cell);
            }
            YValues[y] = mix(XValues[0], XValues[1], interp.x);
        }
        ZValues[z] = mix(YValues[0], YValues[1], interp.y);
    }

    float noise = -1.0 + 2.0 * mix(ZValues[0].x, ZValues[1].x, interp.z);
    // float noise = length(noiseVec)/10;
    return noise;
}

vec4 integrate(vec4 sum, float diffuse, float density, vec4 bgcol, float t){
    vec3 lighting = vec3(0.65, 0.68, 0.7) * 1.3 + 0.5 * vec3(0.7, 0.5, 0.3) * diffuse;
    vec3 colrgb = mix(vec3(1.0, 0.95, 0.8), vec3(0.65, 0.65, 0.65), density);
    vec4 col = vec4(colrgb.r, colrgb.g, colrgb.b, density);
    col.rgb *= lighting;
    col.rgb = mix(col.rgb, bgcol.rgb, 1.0 - exp(-0.003 * t * t));
    col.a *= 0.5;
    col.rgb *= col.a;
    return sum + col * (1.0 - sum.a);
}

#define MARCH(steps, noiseMap, cameraPos, viewDir, bgcol, sum, depth, t) \
    for (int i = 0; i < steps + 1; i++) \
    { \
        if(t > depth) \
            break; \
        vec3 pos = cameraPos + t * viewDir; \
        if (pos.y < _MinHeight || pos.y > _MaxHeight || sum.a > 0.99) \
        { \
            t += max(0.1, 0.02*t); \
            continue; \
        } \
        float density = noiseMap(pos); \
        if (density > 0.01) \
        { \
            float diffuse = clamp((density - noiseMap(pos + 0.3 * _SunDir.xyz)) / 0.6, 0.0, 1.0); \
            sum = integrate(sum, diffuse, density, bgcol, t); \
        } \
        t += max(0.1, 0.02 * t); \
    }

#define NOISEPROC(N, P) 1.75 * N * clamp((_MaxHeight - P.y) / _FadeDistance, 0.0, 1.0)

float map1(vec3 q){
    vec3 p = q;
    float f;
    f = 0.5 * noise3d(q);
    return NOISEPROC(f, p);
}

vec4 rayMarch(vec3 cameraPos, vec3 viewDir, vec4 bgcol, float depth){
    vec4 col = vec4(0, 0, 0, 0);
    float ct = 0;
    MARCH(int(_Steps), map1, cameraPos, viewDir, bgcol, col, depth, ct);
    return clamp(col, 0.0, 1.0);
}

void main(){
    float depth = 1;
    depth *= length(FragPos - viewPos);
    vec4 col = vec4(1, 1, 1, 1);
    vec4 clouds = rayMarch(viewPos, normalize(FragPos - viewPos) * _StepScale, col, depth);
    vec3 mixedColor = col.rgb * (1.0 - clouds.a) + clouds.rgb;
    FragColor = vec4(mixedColor, clouds.a);
}
