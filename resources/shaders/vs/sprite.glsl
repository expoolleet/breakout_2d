#version 450 core

layout (location = 0) in vec4 vertexData;

out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vertexData.x, vertexData.y, 0.0, 1.0);
        TexCoords = vertexData.zw;
}
