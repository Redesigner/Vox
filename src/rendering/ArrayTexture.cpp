#include "ArrayTexture.h"

#include "external/glad.h"
#include "raylib.h"
#include "rlgl.h"

// https://www.khronos.org/opengl/wiki/Array_Texture

ArrayTexture::ArrayTexture(unsigned int width, unsigned int height, unsigned int layerCount, unsigned int mipLevels)
	: width(width), height(height), layerCount(layerCount)
{
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_RGBA8, width, height, layerCount);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

ArrayTexture::~ArrayTexture()
{
	glDeleteTextures(1, &textureId);
}

void ArrayTexture::LoadTexture(std::string textureName, unsigned int index)
{
	if (index >= layerCount)
	{
		TraceLog(LOG_ERROR, "ArrayTexture: attempted to loadtexture outside of ArrayTexture range.");
		return;
	}

	// Load the image onto the cpu, so we can add it to the array
	Image image = LoadImage(textureName.c_str());
	if (!IsImageValid(image))
	{
		TraceLog(LOG_WARNING, "ArrayTexture failed to load.");
		return;
	}

	if (image.width != width || image.height != height)
	{
		TraceLog(LOG_WARNING, "ArrayTexture: loaded image dimensions did not match the ArrayTexture dimensions.");
		UnloadImage(image);
		return;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, image.data);

	// Unload the image, now that it's on the GPU
	UnloadImage(image);
}

unsigned int ArrayTexture::GetId() const
{
	return textureId;
}
