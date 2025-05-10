#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <string>

namespace Vox
{
    class Framebuffer
    {
    public:
        Framebuffer(int width, int height);
        ~Framebuffer();

        [[nodiscard]] unsigned int GetFramebufferId() const;

        [[nodiscard]] glm::ivec2 GetSize() const;

        [[nodiscard]] int GetWidth() const;
        [[nodiscard]] int GetHeight() const;

        static std::string GetFramebufferStatusString(unsigned int framebufferStatus);
        
    protected:
        void CreateFrameBuffer();

        /**
         * @brief Create a texture for this framebuffer. Textures must be created and destroyed manually
         * See <a href="https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml">glTexImage2D</a> for
         * format reference.
         * @param textureId Generated texture as an int
         * @param attachment Framebuffer color attachment index, between range 0 and GL_MAX_COLOR_ATTACHMENT
         * @param internalFormat OpenGl internal format
         * @param format OpenGl format, usually GL_RGB, GL_RGBA, GL_RED, etc.
         * @param type Pixel data type, usually GL_FLOAT
         */
        void BindTexture(unsigned int textureId, unsigned int attachment, GLint internalFormat, GLenum format, GLenum type) const;

        /**
         * @brief Create and attach a depth renderbuffer to the framebuffer, with a 32-bit float depth channel.
         * The depth buffer will be automatically deleted when the framebuffer is destroyed.
         */
        void AttachDepthBuffer();

        static bool CheckStatus();

        unsigned int framebufferId = 0;

        unsigned int depthRenderbuffer = 0;

        int width, height;
    };
}
