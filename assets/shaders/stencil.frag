#version 330 core

layout (location = 0) out uint stencil;

void main() {
    stencil = 1U;
}