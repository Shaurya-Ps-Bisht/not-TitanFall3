#version 410 core

in float Height;
uniform sampler2D uHeightMap;

uniform vec2 uTexelSize;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    float HEIGHT_SCALE = 32.0;
    float left  = texture(uHeightMap, TexCoords + vec2(-uTexelSize.x, 0.0)).r * HEIGHT_SCALE * 2.0 - 16.0;
    float right = texture(uHeightMap, TexCoords + vec2( uTexelSize.x, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float up    = texture(uHeightMap, TexCoords + vec2(0.0,  uTexelSize.y)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float down  = texture(uHeightMap, TexCoords + vec2(0.0, -uTexelSize.y)).r * HEIGHT_SCALE * 2.0 - 1.0;
    vec3 normal = normalize(vec3(down - up, 2.0, left - right));

    float h = (Height + 16)/64.0f;
    FragColor = vec4(h, h, h, 1.0);

    // FragColor = vec4(normal, 1.0);
    // FragColor = texture(uHeightMap, TexCoords + vec2(0.0, -uTexelSize))* 64.0 - 16.0;
    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);

}