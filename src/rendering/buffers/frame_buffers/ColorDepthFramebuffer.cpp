#include "ColorDepthFramebuffer.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

namespace Vox
{
	ColorDepthFramebuffer::ColorDepthFramebuffer(const int width, const int height)
		:Framebuffer(width, height)
	{
		unsigned int textureIds[2] = {};
		glGenTextures(2, textureIds);

		colorTexture = textureIds[0];
		depthTexture = textureIds[1];

	    BindTexture(colorTexture, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	    BindTexture(depthTexture, 4, GL_R32F, GL_RED, GL_FLOAT);
        AttachDepthBuffer();

        constexpr GLenum buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT4};
        glDrawBuffers(2, buffers);

		CheckStatus();
	}

	ColorDepthFramebuffer::~ColorDepthFramebuffer()
	{
		const unsigned int textureIds[2] = { colorTexture, depthTexture};
		glDeleteTextures(2, textureIds);
	}

	void ColorDepthFramebuffer::ActivateTextures(const unsigned int offset) const
	{
		glActiveTexture(GL_TEXTURE0 + offset);
		glBindTexture(GL_TEXTURE_2D, colorTexture);

		glActiveTexture(GL_TEXTURE4 + offset);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	unsigned int ColorDepthFramebuffer::GetTextureId() const
	{
		return colorTexture;
	}
}