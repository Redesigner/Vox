//
// Created by steph on 6/8/2025.
//

#pragma once
#include <filesystem>

namespace Vox
{
    struct AssetPtr
    {
        enum class AssetType : char
        {
            Mesh,
            SkeletalMesh,
            Texture
        };

        AssetType type;
        std::filesystem::path path;
    };
} // Vox
