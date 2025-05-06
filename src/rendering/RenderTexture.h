#pragma once

namespace Vox
{
	class RenderTexture
	{
	public:
		RenderTexture(int width, int height);
		~RenderTexture();

		unsigned int GetFramebufferId() const;
		unsigned int GetTextureId() const;

		int GetWidth() const;
		int GetHeight() const;

		double GetAspectRatio() const;

	private:
		unsigned int framebuffer, texture;
		int width, height;
	};
}