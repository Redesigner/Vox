#version 330 core

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec3 vertexColor;

uniform mat4 viewProjection;
out vec3 fragColor;

void main() {
    gl_Position = viewProjection * vec4(vertexPosition.xyz, 1.0);
    fragColor = vertexColor;
}