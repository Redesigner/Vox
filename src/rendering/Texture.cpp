#include "Texture.h"

#include <GL/glew.h>
#include <SDL3_Image/SDL_image.h>

#include "core/logging/Logging.h"

namespace Vox
{
	Texture::Texture()
		:width(0), height(0), textureId(0)
	{
	}

	Texture::~Texture()
	{
		if (loaded)
		{
			glDeleteTextures(1, &textureId);
		}
	}

	bool Texture::Load(std::string filename)
	{
		SDL_Surface* surface = IMG_Load(filename.c_str());
		if (!surface)
		{
			VoxLog(Warning, FileSystem, "Failed to load image.");
			return false;
		}

		width = surface->w;
		height = surface->h;

		loaded = true;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

		SDL_DestroySurface(surface);

		return true;
	}

	unsigned int Texture::GetWidth() const
	{
		return width;
	}

	unsigned int Texture::GetHeight() const
	{
		return height;
	}

	unsigned int Texture::GetId() const
	{
		return textureId;
	}
}