#pragma once

#include <string>

namespace Vox
{
	class Framebuffer
	{
	public:
		Framebuffer(int width, int height);
		~Framebuffer();

		void ActivateTextures() const;

		unsigned int GetFramebufferId() const;

		static std::string GetFramebufferStatusString(unsigned int framebufferStatus);

	private:
		int width, height;
		unsigned int framebuffer, colorTexture, depthTexture, depthRenderbuffer;
	};
}