#version 450 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D sprite;

void main() {
    vec4 sampled = texture(sprite, TexCoords);
    FragColor = sampled * ParticleColor;
}