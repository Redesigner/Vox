#include "GBuffer.h"

#include <string>

#include <fmt/core.h>
#include <GL/glew.h>

#include "core/logging/Logging.h"
#include "rendering/Framebuffer.h"

namespace Vox
{
	GBuffer::GBuffer(int width, int height)
		:width(width), height(height)
	{
		VoxLog(Display, Rendering, "Allocating GBuffer: size ({}, {})...", width, height);

		if (width == 0 || height == 0)
		{
			framebuffer = 0;
			positionTexture = normalTexture = albedoTexture = metallicRoughnessTexture = depthTexture = depthRenderbuffer = 0;
			VoxLog(Warning, Rendering, "Failed to create GBuffer, width or height cannot be 0.");
			return;
		}

		glGenFramebuffers(1, &framebuffer);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);   // Unbind any framebuffer

		unsigned int textureIds[5] = {};
		glGenTextures(5, textureIds);

		positionTexture = textureIds[0];
		normalTexture = textureIds[1];
		albedoTexture = textureIds[2];
		metallicRoughnessTexture = textureIds[3];
		depthTexture = textureIds[4];

		glGenRenderbuffers(1, &depthRenderbuffer);

		// Position buffer
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

		// Normal buffer
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

		// Albedo buffer
		glBindTexture(GL_TEXTURE_2D, albedoTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTexture, 0);

		// Metallic/roughness buffer
		glBindTexture(GL_TEXTURE_2D, metallicRoughnessTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, metallicRoughnessTexture, 0);

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

		// VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);

		GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			VoxLog(Error, Rendering, "Failed to create GBuffer: {}", Framebuffer::GetFramebufferStatusString(framebufferStatus));
		}
		else
		{
			VoxLog(Display, Rendering, "Successfully created GBuffer.");
		}
	}

	GBuffer::~GBuffer()
	{
		VoxLog(Display, Rendering, "Destroying GBuffer");

		unsigned int textureIds[5] = { positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture };
		glDeleteTextures(5, textureIds);
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		glDeleteFramebuffers(1, &framebuffer);
	}

	void GBuffer::ActivateTextures(unsigned int offset) const
	{
		glActiveTexture(GL_TEXTURE0 + offset);
		glBindTexture(GL_TEXTURE_2D, positionTexture);

		glActiveTexture(GL_TEXTURE1 + offset);
		glBindTexture(GL_TEXTURE_2D, normalTexture);

		glActiveTexture(GL_TEXTURE2 + offset);
		glBindTexture(GL_TEXTURE_2D, albedoTexture);

		glActiveTexture(GL_TEXTURE3 + offset);
		glBindTexture(GL_TEXTURE_2D, metallicRoughnessTexture);

		glActiveTexture(GL_TEXTURE4 + offset);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	void GBuffer::CopyToFramebuffer(unsigned int targetFramebuffer)
	{
		glBlitNamedFramebuffer(framebuffer, targetFramebuffer, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_LINEAR);
	}

	unsigned int GBuffer::GetFramebufferId() const
	{
		return framebuffer;
	}
};