#pragma once

#include <string>
#include <vector>

class ArrayTexture
{
public:
	ArrayTexture(unsigned int width, unsigned int height, unsigned int layerCount, unsigned int mipLevels);
	~ArrayTexture();

	void LoadTexture(std::string textureName, unsigned int index);

	unsigned int GetId() const;

private:
	unsigned int textureId = 0;
	unsigned int width, height, layerCount;
};