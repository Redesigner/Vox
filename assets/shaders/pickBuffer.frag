#version 330 core

uniform uint objectId;
layout (location = 0) out uint object;

void main() {
    object = objectId;
}