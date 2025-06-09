//
// Created by steph on 6/9/2025.
//

#pragma once
#include <string>

namespace Vox
{
    struct AssetPtr;

    class AssetPtrDetailPanel
    {
    public:
        static bool Draw(AssetPtr& assetPtr, const std::string& label);
    };

} // Vox
