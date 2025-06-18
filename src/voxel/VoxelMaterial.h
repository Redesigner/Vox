//
// Created by steph on 6/18/2025.
//

#pragma once

namespace Vox
{

    struct VoxelMaterial
    {
        using uint = unsigned int;

        explicit VoxelMaterial(uint texture);
        VoxelMaterial(uint top, uint bottom, uint sides);
        VoxelMaterial(uint front, uint back, uint left, uint right, uint top, uint bottom);

        uint front;
        uint back;
        uint left;
        uint right;
        uint top;
        uint bottom;
    };

} // Vox
