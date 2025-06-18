//
// Created by steph on 6/18/2025.
//

#include "VoxelMaterial.h"

namespace Vox
{
    VoxelMaterial::VoxelMaterial(const uint texture)
        :front(texture), back(texture), left(texture), right(texture), top(texture), bottom(texture)
    {
    }

    VoxelMaterial::VoxelMaterial(const uint top, const uint bottom, const uint sides)
        :front(sides), back(sides), left(sides), right(sides), top(top), bottom(bottom)
    {
    }

    VoxelMaterial::VoxelMaterial(const uint front, const uint back, const uint left, const uint right, const uint top, const uint bottom)
        :front(front), back(back), left(left), right(right), top(top), bottom(bottom)
    {
    }
} // Vox