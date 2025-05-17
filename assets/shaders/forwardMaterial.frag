#version 330 core

uniform vec4 materialAlbedo;
uniform float materialRoughness;
uniform float materialMetallic;

void main() {
    gl_FragColor = vec4(materialAlbedo.rgb, 1.0);
}