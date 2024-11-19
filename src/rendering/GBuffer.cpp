#include "GBuffer.h"

#include "raylib.h"
#include "rlgl.h"

GBuffer::GBuffer(int width, int height)
	:width(width), height(height)
{
	TraceLog(LOG_INFO, "Allocating GBuffer");

	framebuffer = rlLoadFramebuffer();
	if (!framebuffer)
	{
		TraceLog(LOG_WARNING, "Failed to create framebuffer");
	}

	rlEnableFramebuffer(framebuffer);

	positionTexture =	rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	normalTexture =		rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	albedoSpecTexture = rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
	depthTexture =		rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32, 1);

	rlActiveDrawBuffers(4);

	rlFramebufferAttach(framebuffer, positionTexture,	RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, normalTexture,		RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, albedoSpecTexture, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, depthTexture,		RL_ATTACHMENT_COLOR_CHANNEL3, RL_ATTACHMENT_TEXTURE2D, 0);

	depthRenderbuffer = rlLoadTextureDepth(width, height, true);
	rlFramebufferAttach(framebuffer, depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

	if (!rlFramebufferComplete(framebuffer))
	{
		TraceLog(LOG_ERROR, "Failed to create framebuffer.");
	}
}

GBuffer::~GBuffer()
{
	TraceLog(LOG_INFO, "Destroying GBuffer");
	rlUnloadFramebuffer(framebuffer);
	rlUnloadTexture(positionTexture);
	rlUnloadTexture(normalTexture);
	rlUnloadTexture(albedoSpecTexture);
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
	rlEnableTexture(albedoSpecTexture);

	rlActiveTextureSlot(3 + offset);
	rlEnableTexture(depthTexture);
}

void GBuffer::CopyToFramebuffer(const RenderTexture2D& target)
{
	rlBindFramebuffer(RL_READ_FRAMEBUFFER, framebuffer);
	rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, target.id);
	rlBlitFramebuffer(0, 0, width, height, 0, 0, width, height, 0x00000100);
}