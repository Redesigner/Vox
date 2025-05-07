#pragma once

#include <string>

namespace Vox
{
    class Texture
    {
    public:
        Texture();
        ~Texture();

        bool Load(std::string filename);

        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
        unsigned int GetId() const;

    private:
        unsigned int width, height;
        unsigned int textureId;
        bool loaded = false;
    };
}
