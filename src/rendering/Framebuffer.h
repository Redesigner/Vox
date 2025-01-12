#pragma once

namespace Vox
{
	class Framebuffer
	{
	public:
		Framebuffer(int width, int height);
		~Framebuffer();

		void ActivateTextures() const;

		unsigned int GetFramebufferId() const;

	private:
		int width, height;
		unsigned int framebuffer, colorTexture, depthTexture, depthRenderbuffer;
	};
}