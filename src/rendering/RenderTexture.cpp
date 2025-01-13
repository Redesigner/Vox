#include "RenderTexture.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"
#include "rendering/Framebuffer.h"

namespace Vox
{
	RenderTexture::RenderTexture(unsigned int width, unsigned int height)
		:width(width), height(height)
	{
		VoxLog(Display, Rendering, "Allocating new framebuffer for RenderTexture: size ({}, {}).", width, height);

		if (width == 0 || height == 0)
		{
			framebuffer = texture = 0;
			VoxLog(Warning, Rendering, "Failed to create renderTexture, width or height cannot be 0.");
			return;
		}

		glGenFramebuffers(1, &framebuffer);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			VoxLog(Error, Rendering, "Failed to create renderTexture: {}", Framebuffer::GetFramebufferStatusString(framebufferStatus));
		}
	}

	RenderTexture::~RenderTexture()
	{
		glDeleteTextures(1, &texture);
		glDeleteFramebuffers(1, &framebuffer);
	}

	unsigned int RenderTexture::GetFramebufferId() const
	{
		return framebuffer;
	}

	unsigned int RenderTexture::GetTextureId() const
	{
		return texture;
	}

	unsigned int RenderTexture::GetWidth() const
	{
		return width;
	}

	unsigned int RenderTexture::GetHeight() const
	{
		return height;
	}
}