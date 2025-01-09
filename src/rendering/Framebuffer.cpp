#include "FrameBuffer.h"

#include "core/logging/Logging.h"

#include "raylib.h"
#include "rlgl.h"

Framebuffer::Framebuffer(int width, int height)
	:width(width), height(height)
{
	VoxLog(Display, Rendering, "Allocating custom FrameBuffer.");

	framebuffer = rlLoadFramebuffer();
	if (!framebuffer)
	{
		VoxLog(Warning, Rendering, "Failed to create framebuffer");
	}

	rlEnableFramebuffer(framebuffer);

	colorTexture = rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
	depthTexture = rlLoadTexture(NULL, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32, 1);
	rlActiveDrawBuffers(2);
	rlFramebufferAttach(framebuffer, colorTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
	rlFramebufferAttach(framebuffer, depthTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);

	depthRenderbuffer = rlLoadTextureDepth(width, height, true);
	rlFramebufferAttach(framebuffer, depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

	if (!rlFramebufferComplete(framebuffer))
	{
		VoxLog(Warning, Rendering, "Failed to create framebuffer.");
	}
}

Framebuffer::~Framebuffer()
{
	VoxLog(Display, Rendering, "Destroying GBuffer");
	rlUnloadFramebuffer(framebuffer);
	rlUnloadTexture(colorTexture);
	rlUnloadTexture(depthTexture);
	rlUnloadTexture(depthRenderbuffer);
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
