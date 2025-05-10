#pragma once

#include "rendering/buffers/frame_buffers/Framebuffer.h"

namespace Vox
{
	class ColorDepthFramebuffer : public Framebuffer
	{
	public:
		ColorDepthFramebuffer(int width, int height);
		~ColorDepthFramebuffer();

		void ActivateTextures(unsigned int offset) const;

		[[nodiscard]] unsigned int GetTextureId() const;

	private:
		unsigned int colorTexture, depthTexture;
	};
}