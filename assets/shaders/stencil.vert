#version 330 core
layout (location = 0) in vec3 vertexPosition;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);
    gl_Position = matProjection * matView * worldPos;
}