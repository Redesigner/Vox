#include "SimpleFramebuffer.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

namespace Vox
{
	SimpleFramebuffer::SimpleFramebuffer(int width, int height)
		:width(width), height(height)
	{
		VoxLog(Display, Rendering, "Allocating FrameBuffer: size({}, {})", width, height);

		if (width == 0 || height == 0)
		{
			framebuffer = colorTexture = depthTexture = depthRenderbuffer = 0;
			VoxLog(Warning, Rendering, "Failed to create FrameBuffer, width or height cannot be 0.");
			return;
		}

		glGenFramebuffers(1, &framebuffer);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		unsigned int textureIds[2] = {};
		glGenTextures(2, textureIds);

		colorTexture = textureIds[0];
		depthTexture = textureIds[1];

		//VoxLog(Display, Rendering, "Allocated Framebuffer textures '({}, {})'", colorTexture, depthTexture);

		glGenRenderbuffers(1, &depthRenderbuffer);

		// Color buffer
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

		// Depth buffer (for reading)
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, depthTexture, 0);

		// Alternate depth texture
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			VoxLog(Error, Rendering, "Failed to create Framebuffer: {}", SimpleFramebuffer::GetFramebufferStatusString(framebufferStatus));
		}
	}

	SimpleFramebuffer::~SimpleFramebuffer()
	{
		VoxLog(Display, Rendering, "Destroying Framebuffer...");
		//VoxLog(Display, Rendering, "Destroying Framebuffer textures '({}, {})'", colorTexture, depthTexture);

		unsigned int textureIds[2] = { colorTexture, depthTexture};
		glDeleteTextures(2, textureIds);
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		glDeleteFramebuffers(1, &framebuffer);
	}

	void SimpleFramebuffer::ActivateTextures(const unsigned int offset) const
	{
		glActiveTexture(GL_TEXTURE0 + offset);
		glBindTexture(GL_TEXTURE_2D, colorTexture);

		glActiveTexture(GL_TEXTURE1 + offset);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	unsigned int SimpleFramebuffer::GetFramebufferId() const
	{
		return framebuffer;
	}

	unsigned int SimpleFramebuffer::GetTextureId() const
	{
		return colorTexture;
	}

	std::string SimpleFramebuffer::GetFramebufferStatusString(unsigned int framebufferStatus)
	{
		switch (framebufferStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			return "GL_FRAMEBUFFER_UNDEFINED";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return "GL_FRAMEBUFFER_UNSUPPORTED";
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		default:
			return "UNKNOWN";
		}
	}

	unsigned int SimpleFramebuffer::GetWidth() const
	{
		return width;
	}

	unsigned int SimpleFramebuffer::GetHeight() const
	{
		return height;
	}
}