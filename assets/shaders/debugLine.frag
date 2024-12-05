#version 330 core

in vec3 fragNormal;
in vec3 fragColor;

const vec3 lightDirection = normalize(vec3(0.0, 1.0, 1.0));

void main() {
    float strength = max(dot(lightDirection, fragNormal) + 0.5, 0.25);
    gl_FragColor = vec4(fragColor * strength, 1.0);
}