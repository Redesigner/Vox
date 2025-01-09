#include "GBuffer.h"

#include "raylib.h"
#include "rlgl.h"
#include <fmt/core.h>
#include <string>

#include "core/logging/Logging.h"

GBuffer::GBuffer(int width, int height)
	:width(width), height(height)
{
	VoxLog(Display, Rendering, "Allocating GBuffer");

	framebuffer = rlLoadFramebuffer();
	if (!framebuffer)
	{
		VoxLog(Warning, Rendering, "Failed to create framebuffer");
	}

	rlEnableFramebuffer(framebuffer);

	positionTexture =			rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	normalTexture =				rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	albedoTexture =				rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8, 1);
	metallicRoughnessTexture =	rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8, 1);
	depthTexture =				rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32, 1);

	VoxLog(Display, Rendering, "Position {}, Normal {}, AlbedoSpec {}, Depth {}", positionTexture, normalTexture, albedoTexture, depthTexture);

	rlActiveDrawBuffers(5);

	rlFramebufferAttach(framebuffer, positionTexture,				RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, normalTexture,					RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, albedoTexture,					RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, metallicRoughnessTexture,		RL_ATTACHMENT_COLOR_CHANNEL3, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, depthTexture,					RL_ATTACHMENT_COLOR_CHANNEL4, RL_ATTACHMENT_TEXTURE2D, 0);

	depthRenderbuffer = rlLoadTextureDepth(width, height, true);
	rlFramebufferAttach(framebuffer, depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

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

void GBuffer::CopyToFramebuffer(const RenderTexture2D& target)
{
	rlBindFramebuffer(RL_READ_FRAMEBUFFER, framebuffer);
	rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, target.id);
	rlBlitFramebuffer(0, 0, width, height, 0, 0, width, height, 0x00000100);
}