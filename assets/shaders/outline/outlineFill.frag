#version 330 core

out ivec2 pixelOrigin;
in vec2 texCoord;

uniform usampler2D stencil;

uniform vec2 fragToPixel;
uniform vec2 pixelToFrag;

void main()
{
    ivec2 currentPixel = ivec2(gl_FragCoord.xy);
    uint stencilId = texelFetch(stencil, currentPixel, 0).r;
    bool edge = int(stencilId) > 0;

    bool foundNonStenciledPixel = false;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            ivec2 samplePixel = currentPixel + ivec2(x, y);
            uint sampleId = texelFetch(stencil, samplePixel, 0).r;

            if (samplePixel.x < 0 || samplePixel.y < 0 || samplePixel.x >= int(fragToPixel.x) || samplePixel.y >= int(fragToPixel.y))
            {
                continue;
            }

            if (int(sampleId) == 0)
            {
                foundNonStenciledPixel = true;
            }
        }
    }

    pixelOrigin = edge && foundNonStenciledPixel ? currentPixel : ivec2(-1, -1);
}