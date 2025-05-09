#version 330 core

out vec4 finalColor;
in vec2 texCoord;

uniform int width;
uniform vec3 outlineColor;
uniform isampler2D previousStage;
uniform sampler2D color;

void main()
{
    ivec2 currentPixel = ivec2(gl_FragCoord.xy);
    ivec2 originPixel = ivec2(texelFetch(previousStage, currentPixel, 0).rg);

    float pixelDistance = length(vec2(currentPixel - originPixel));

    finalColor = (originPixel.x >= 0 && pixelDistance < width) ? vec4(outlineColor, 1.0) : texture(color, texCoord);
}