//
// Created by steph on 6/18/2025.
//

#include "VoxelGenerationShader.h"

#include "voxel/VoxelMaterial.h"

namespace Vox
{
    VoxelGenerationShader::VoxelGenerationShader()
        :ComputeShader("assets/shaders/voxelGeneration.comp")
    {
        uniformLocations.materialId = GetUniformLocation("materialId");

        uniformLocations.front = GetUniformLocation("material.front");
        uniformLocations.back = GetUniformLocation("material.back");
        uniformLocations.top = GetUniformLocation("material.top");
        uniformLocations.bottom = GetUniformLocation("material.bottom");
        uniformLocations.left = GetUniformLocation("material.left");
        uniformLocations.right = GetUniformLocation("material.right");
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void VoxelGenerationShader::SetVoxelMaterial(const unsigned int id, const VoxelMaterial& material)
    {
        SetUniformUint(uniformLocations.materialId, id);

        SetUniformUint(uniformLocations.front, material.front);
        SetUniformUint(uniformLocations.back, material.back);
        SetUniformUint(uniformLocations.top, material.top);
        SetUniformUint(uniformLocations.bottom, material.bottom);
        SetUniformUint(uniformLocations.left, material.left);
        SetUniformUint(uniformLocations.right, material.right);
    }
} // Vox