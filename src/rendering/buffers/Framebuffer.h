#pragma once
#include <GL/glew.h>

namespace Vox
{
    class Framebuffer
    {
    public:
        Framebuffer(int width, int height);
        ~Framebuffer();

        unsigned int GetFramebufferId() const;
        
    protected:
        void CreateFrameBuffer();

        void BindTexture(unsigned int textureId, unsigned int attachment, GLint internalFormat, GLenum format, GLenum type) const;

        unsigned int framebufferId = 0;

        int width, height;
    };
}
