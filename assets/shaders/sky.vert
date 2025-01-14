#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;

uniform mat4 cameraWorldSpace;

out vec2 texCoord;
out vec4 vertexWS;

void main() {
    gl_Position = vec4(vertexPosition.xy, 0.999999, 1.0);
    texCoord = vertexTexCoord;
    vertexWS = cameraWorldSpace * gl_Position;
    // vertexWS = vec4(vertexPosition.xy, 1.0, 1.0);
}