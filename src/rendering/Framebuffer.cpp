#include "FrameBuffer.h"

#include "raylib.h"
#include "rlgl.h"

Framebuffer::Framebuffer(int width, int height)
	:width(width), height(height)
{
	TraceLog(LOG_INFO, "Allocating custom FrameBuffer.");

	framebuffer = rlLoadFramebuffer();
	if (!framebuffer)
	{
		TraceLog(LOG_WARNING, "Failed to create framebuffer");
	}

	rlEnableFramebuffer(framebuffer);

	colorTexture = rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	depthTexture = rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32, 1);
	rlActiveDrawBuffers(2);
	rlFramebufferAttach(framebuffer, colorTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, depthTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);

	if (!rlFramebufferComplete(framebuffer))
	{
		TraceLog(LOG_ERROR, "Failed to create framebuffer.");
	}
}

Framebuffer::~Framebuffer()
{
	TraceLog(LOG_INFO, "Destroying GBuffer");
	rlUnloadFramebuffer(framebuffer);
	rlUnloadTexture(colorTexture);
	rlUnloadTexture(depthTexture);
}

void Framebuffer::EnableFramebuffer()
{
	rlEnableFramebuffer(framebuffer);
}

void Framebuffer::BindDraw()
{
	rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, framebuffer);
}

void Framebuffer::BindRead()
{
	rlBindFramebuffer(RL_READ_FRAMEBUFFER, framebuffer);
}

void Framebuffer::ActivateTextures() const
{
	rlActiveTextureSlot(0);
	rlEnableTexture(colorTexture);
	rlActiveTextureSlot(1);
	rlEnableTexture(depthTexture);
}
