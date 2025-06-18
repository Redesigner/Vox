//
// Created by steph on 6/18/2025.
//

#pragma once
#include "ComputeShader.h"

namespace Vox
{
    struct VoxelMaterial;

    class VoxelGenerationShader : public ComputeShader
    {
        struct UniformLocations
        {
            int materialId = -1;

            int front = -1;
            int back = -1;
            int left = -1;
            int right = -1;
            int top = -1;
            int bottom = -1;
        };

    public:
        VoxelGenerationShader();

        void SetVoxelMaterial(unsigned int id, const VoxelMaterial& material);

    private:
        UniformLocations uniformLocations;
    };

} // Vox
