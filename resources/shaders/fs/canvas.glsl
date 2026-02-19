#version 450 core

layout (binding = 0) uniform sampler2D color1;

in vec2 TexCoords;
out vec4 FragColor;

uniform vec2 inverseScreenSize;

vec4 tonemap(vec3 color, float exposure);
vec3 applyBC(vec3 color, float b, float c);
vec3 saturate(vec3 color, float saturation);
float createVignette(float intensity, float radius, float softness);
vec3 applyChromaticAberration(sampler2D tex);

vec3 lumaWeights = vec3(0.299, 0.587, 0.114);

void main() {
    vec3 postColor = applyBC(texture(color1, TexCoords).rgb, 0.9, 1.4);
    postColor = saturate(postColor, 1.2);
    postColor *= createVignette(0.5, 1.5, 0.6);
    FragColor = tonemap(postColor, 2.2);
}

vec4 tonemap(vec3 color, float exposure) {
    return vec4(vec3(1.0) - exp(-color * exposure), 1.0);
}

vec3 applyBC(vec3 color, float b, float c) {
    color *= b;
    color = (color - 0.5) * c + 0.5;
    return max(color, 0.0);
}

vec3 saturate(vec3 color, float saturation) {
    float luma = dot(color, lumaWeights);
    return mix(vec3(luma), color, saturation);
}

float createVignette(float intensity, float radius, float softness) {
    vec2 distance = TexCoords - vec2(0.5);
    float sdf = length(distance * vec2(1.5 / (inverseScreenSize.x / inverseScreenSize.y), 1.5));
    float vignette = smoothstep(radius, radius - softness, sdf);
    return mix(1.0 - intensity, 1.0, vignette);
}

vec3 applyChromaticAberration(sampler2D tex) {
    float r = texture(tex, TexCoords + vec2(-2.0, 0.0) * inverseScreenSize).r;
    float g = texture(tex, TexCoords + vec2(2.0, 2.0) * inverseScreenSize).g;
    float b = texture(tex, TexCoords + vec2(-2.0, 2.0) * inverseScreenSize).b;
    return vec3(r, g, b);
}
