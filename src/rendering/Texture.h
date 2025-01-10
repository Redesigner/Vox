#pragma once

namespace Vox
{
	class Texture
	{
	public:
		Texture(unsigned int width, unsigned int height);
		~Texture();

	private:
		unsigned int width, height;
		unsigned int textureId;
	};
}