#include "VoxelBody.h"

namespace Vox
{
    VoxelBody::VoxelBody(unsigned int dimensions)
        :voxelCollisionMask(dimensions)
    {
    }

    void VoxelBody::SetVoxel(glm::uvec3 position)
    {
    }

    void VoxelBody::EraseVoxel(glm::uvec3 position)
    {
    }

    JPH::Ref<JPH::StaticCompoundShapeSettings> VoxelBody::GetShapeSettings()
    {
        return voxelCollisionMask.MakeCompoundShape();
    }
}