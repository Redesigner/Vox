#pragma once

namespace Vox
{
	class RenderTexture
	{
	public:
		RenderTexture(unsigned int width, unsigned int height);
		~RenderTexture();

		unsigned int GetFramebufferId() const;
		unsigned int GetTextureId() const;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		double GetAspectRatio() const;

	private:
		unsigned int framebuffer, texture;
		unsigned int width, height;
	};
}