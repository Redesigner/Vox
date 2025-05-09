#version 330 core

out ivec2 pixelOrigin;
in vec2 texCoord;

uniform usampler2D stencil;

uniform vec2 fragToPixel;
uniform vec2 pixelToFrag;

void main() {
    int stencilId = int(texture(stencil, texCoord).r);
    pixelOrigin = (stencilId > 0.0) ? ivec2(gl_FragCoord.xy) : ivec2(-1, -1);
}