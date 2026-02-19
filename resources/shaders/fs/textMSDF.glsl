#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

layout (binding = 0) uniform sampler2D textMSDF;
uniform vec3 textColor;
uniform float distanceRange;
uniform float outlineWidth;
uniform vec3 outlineColor;
uniform bool isBold;

float boldThickness = 1.0;

float makeBold(float thickness, float pxDist) {
    return clamp(pxDist + 0.5 + thickness, 0.0, 1.0);
}

float makeOutline(float width, float pxDist) {
    return clamp(pxDist + 0.5 + width, 0.0, 1.0);
}

float makeOutlineSmooth(float width, float pxDist) {
    return smoothstep(0.0, 1.0, pxDist + 0.5 + width) - smoothstep(0.0, 1.0, pxDist + 0.5 - width);
}

float screenPxRange() {
    vec2 unitRange = vec2(distanceRange) / vec2(textureSize(textMSDF, 0));
    vec2 screenRange = unitRange / fwidth(TexCoords);
    return 0.5 * (screenRange.x + screenRange.y);
}

float median(vec3 color) {
    return max(min(color.r, color.g), min(max(color.r, color.g), color.b));
}

void main() {
    vec3 sampled = texture(textMSDF, TexCoords).rgb;

    float dist = median(sampled);
    
    float pxDist = screenPxRange() * (dist - 0.5);

    float textOpacity = isBold ? makeBold(boldThickness, pxDist) : clamp(pxDist + 0.5, 0.0, 1.0);
    
    if (outlineWidth > 0.0) {
        vec3 finalColor = mix(outlineColor, textColor, textOpacity);
        float outlineOpacity = makeOutline(outlineWidth, pxDist);
        FragColor = vec4(finalColor, outlineOpacity);
    } else {
        FragColor = vec4(textColor, textOpacity);
    }
}