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

		unsigned int fboId = 0;
		glGenFramebuffers(1, &fboId);       // Create the framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, 0);   // Unbind any framebuffer

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int textureIds[5] = {};
		glGenTextures(5, textureIds);

		positionTexture = textureIds[0];
		normalTexture = textureIds[1];
		albedoTexture = textureIds[2];
		metallicRoughnessTexture = textureIds[3];
		depthTexture = textureIds[4];

		// Position buffer
		glBindTexture(GL_TEXTURE_2D, positionTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

		// Normal buffer
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, depthTexture, 0);

		// Alternate depth texture
		glBindTexture(GL_TEXTURE_2D, depthRenderbuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderbuffer, 0);

		VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);

		if (!rlFramebufferComplete(framebuffer))
		{
			VoxLog(Error, Rendering, "Failed to create framebuffer.");
		}
	}

	GBuffer::~GBuffer()
	{
		VoxLog(Display, Rendering, "Destroying GBuffer");
		rlUnloadFramebuffer(framebuffer);
		rlUnloadTexture(positionTexture);
		rlUnloadTexture(normalTexture);
		rlUnloadTexture(albedoTexture);
		rlUnloadTexture(depthRenderbuffer);
	}

	void GBuffer::EnableFramebuffer()
	{
		rlEnableFramebuffer(framebuffer);
	}

	void GBuffer::BindDraw()
	{
		rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, framebuffer);
	}

	void GBuffer::BindRead()
	{
		rlBindFramebuffer(RL_READ_FRAMEBUFFER, framebuffer);
	}

	void GBuffer::ActivateTextures(unsigned int offset) const
	{
		rlActiveTextureSlot(0 + offset);
		rlEnableTexture(positionTexture);

		rlActiveTextureSlot(1 + offset);
		rlEnableTexture(normalTexture);

		rlActiveTextureSlot(2 + offset);
		rlEnableTexture(albedoTexture);

		rlActiveTextureSlot(3 + offset);
		rlEnableTexture(metallicRoughnessTexture);

		rlActiveTextureSlot(4 + offset);
		rlEnableTexture(depthTexture);
	}

	void GBuffer::CopyToFramebuffer(const Texture& target)
	{
		rlBindFramebuffer(RL_READ_FRAMEBUFFER, framebuffer);
		rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, target.id);
		rlBlitFramebuffer(0, 0, width, height, 0, 0, width, height, 0x00000100);
	}
};