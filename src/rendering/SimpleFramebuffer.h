#pragma once

#include <string>

namespace Vox
{
	class SimpleFramebuffer
	{
	public:
		SimpleFramebuffer(int width, int height);
		~SimpleFramebuffer();

		void ActivateTextures(unsigned int offset) const;

		unsigned int GetFramebufferId() const;

		unsigned int GetTextureId() const;

		static std::string GetFramebufferStatusString(unsigned int framebufferStatus);

		unsigned int GetWidth() const;

		unsigned int GetHeight() const;

	private:
		int width, height;
		unsigned int framebuffer, colorTexture, depthTexture, depthRenderbuffer;
	};
}