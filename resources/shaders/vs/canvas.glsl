#version 450 core

layout (location = 0) in vec4 vertexData;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(vertexData.x, vertexData.y, 0.0, 1.0f);
    TexCoords = vertexData.zw;
}
