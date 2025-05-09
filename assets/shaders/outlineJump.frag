#version 330 core

out ivec2 pixelOrigin;
in vec2 texCoord;

uniform int width;
uniform isampler2D previousStage;

void main()
{
    ivec2 currentPixel = ivec2(gl_FragCoord.xy);
    ivec2 currentValue = ivec2(texelFetch(previousStage, currentPixel, 0).rg);

    float shortestDistance = 10000.0;
    ivec2 closestPosition = currentValue;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            ivec2 samplePixel = currentPixel + ivec2(x * width, y * width);
            ivec2 sampleOrigin = ivec2(texelFetch(previousStage, samplePixel, 0).rg);

            if (sampleOrigin.x <= 0)
            {
                continue;
            }

            float distanceToPoint = length(vec2(sampleOrigin - currentPixel));
            if (distanceToPoint < shortestDistance)
            {
                shortestDistance = distanceToPoint;
                closestPosition = sampleOrigin;
            }
        }
    }
    pixelOrigin = (closestPosition.x > 0) ? closestPosition : ivec2(-1, -1);
}