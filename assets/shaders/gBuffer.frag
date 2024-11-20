#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec2 gMetallicRoughness;
layout (location = 4) out float gDepth;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform vec3 materialAlbedo;
uniform float materialRoughness;
uniform float materialMetallic;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main() {
    gPosition = fragPosition;
    gNormal = normalize(fragNormal);
    gAlbedo = materialAlbedo;
    gMetallicRoughness = vec2(materialRoughness, materialMetallic);
    gDepth = gl_FragDepth;
}