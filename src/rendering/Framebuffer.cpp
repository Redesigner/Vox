#include "FrameBuffer.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

namespace Vox
{
	Framebuffer::Framebuffer(int width, int height)
		:width(width), height(height)
	{
		VoxLog(Display, Rendering, "Allocating FrameBuffer.");

		glGenFramebuffers(1, &framebuffer);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		unsigned int textureIds[3] = {};
		glGenTextures(3, textureIds);

		colorTexture = textureIds[0];
		depthTexture = textureIds[1];
		depthRenderbuffer = textureIds[2];

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, depthTexture, 0);

		// Alternate depth texture
		glBindTexture(GL_TEXTURE_2D, depthRenderbuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderbuffer, 0);

		GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			VoxLog(Error, Rendering, "Failed to create framebuffer.");
		}
	}

	Framebuffer::~Framebuffer()
	{
		VoxLog(Display, Rendering, "Destroying FrameBuffer");

		unsigned int textureIds[3] = { colorTexture, depthTexture, depthRenderbuffer };
		glDeleteTextures(6, textureIds);
		glDeleteFramebuffers(1, &framebuffer);
	}

	void Framebuffer::ActivateTextures() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	unsigned int Framebuffer::GetFramebufferId() const
	{
		return framebuffer;
	}
}