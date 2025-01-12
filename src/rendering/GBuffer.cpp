#include "GBuffer.h"

#include <string>

#include <fmt/core.h>
#include <GL/glew.h>

#include "core/logging/Logging.h"

namespace Vox
{
	GBuffer::GBuffer(int width, int height)
		:width(width), height(height)
	{
		VoxLog(Display, Rendering, "Allocating GBuffer");

		glGenFramebuffers(1, &framebuffer);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);   // Unbind any framebuffer

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int textureIds[6] = {};
		glGenTextures(6, textureIds);

		positionTexture = textureIds[0];
		normalTexture = textureIds[1];
		albedoTexture = textureIds[2];
		metallicRoughnessTexture = textureIds[3];
		depthTexture = textureIds[4];
		depthRenderbuffer = textureIds[5];

		// Position buffer
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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
		glBindTexture(GL_TEXTURE_2D, depthRenderbuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderbuffer, 0);

		VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);

		GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			VoxLog(Error, Rendering, "Failed to create framebuffer.");
		}
	}

	GBuffer::~GBuffer()
	{
		VoxLog(Display, Rendering, "Destroying GBuffer");

		unsigned int textureIds[6] = { positionTexture, normalTexture, albedoTexture, metallicRoughnessTexture, depthTexture, depthRenderbuffer };
		glDeleteTextures(6, textureIds);
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