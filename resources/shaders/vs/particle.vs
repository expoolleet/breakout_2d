#version 450 core

layout (location = 0) in vec4 vertexData;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;
uniform float scale;

void main() {
    gl_Position = projection * vec4(vertexData.xy * scale + offset, 0.0, 1.0);
    ParticleColor = color;
    TexCoords = vertexData.zw;
}