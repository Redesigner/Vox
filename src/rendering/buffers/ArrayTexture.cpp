#include "ArrayTexture.h"

#include "core/logging/Logging.h"

#include <GL/glew.h>
#include <SDL3_image/SDL_image.h>

// https://www.khronos.org/opengl/wiki/Array_Texture

namespace Vox
{
    ArrayTexture::ArrayTexture(const unsigned int width, const unsigned int height, const unsigned int layerCount, const unsigned int mipLevels)
        : width(width), height(height), layerCount(layerCount)
    {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_RGBA8, width, height, layerCount);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    ArrayTexture::~ArrayTexture()
    {
        glDeleteTextures(1, &textureId);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ArrayTexture::LoadTexture(std::string textureName, const unsigned int index)
    {
        textureName = "../../../" + textureName;
        if (index >= layerCount)
        {
            VoxLog(Error, Rendering, "ArrayTexture: attempted to load texture outside of ArrayTexture range.");
            return;
        }

        // Load the image onto the cpu, so we can add it to the array
        SDL_Surface* image = IMG_Load(textureName.c_str());
        if (!image)
        {
            VoxLog(Warning, FileSystem, "ArrayTexture failed to load. SDL: {}", SDL_GetError());
            return;
        }

        if (image->w != width || image->h != height)
        {
            VoxLog(Warning, Rendering,
                   "ArrayTexture: loaded image dimensions did not match the ArrayTexture dimensions.");
            SDL_DestroySurface(image);
            return;
        }

        VoxLog(Display, Rendering, "Loaded image {}", textureName);

        glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

        // Unload the image, now that it's on the GPU
        SDL_DestroySurface(image);
    }

    unsigned int ArrayTexture::GetId() const
    {
        return textureId;
    }
}
