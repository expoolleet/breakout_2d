#version 450 core

layout (location = 0) in vec4 vertexData;
layout (location = 1) in vec2 offset;
layout (location = 2) in vec4 color;
layout (location = 3) in float scale;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertexData.xy * scale + offset, 0.0, 1.0);
    ParticleColor = color;
    TexCoords = vertexData.zw;
}
