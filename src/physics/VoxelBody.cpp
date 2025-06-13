#include "VoxelBody.h"

namespace Vox
{
    VoxelBody::VoxelBody(const unsigned int dimensions)
        :voxelCollisionMask(dimensions)
    {
    }

    void VoxelBody::CreateVoxel(const glm::uvec3 position)
    {
        auto solidVoxel = Octree::PhysicsVoxel(true);
        voxelCollisionMask.SetVoxel(
            static_cast<int>(position.x - 16),
            static_cast<int>(position.y - 16),
            static_cast<int>(position.z - 16),
            &solidVoxel);
    }

    void VoxelBody::EraseVoxel(const glm::uvec3 position)
    {
        voxelCollisionMask.SetVoxel(
            static_cast<int>(position.x - 16),
            static_cast<int>(position.y - 16),
            static_cast<int>(position.z - 16),
            nullptr);
    }

    JPH::BodyID VoxelBody::GetBodyId() const
    {
        return bodyId;
    }

    void VoxelBody::SetBodyId(const JPH::BodyID bodyIdIn)
    {
        bodyId = bodyIdIn;
    }

    JPH::Ref<JPH::StaticCompoundShapeSettings> VoxelBody::GetShapeSettings() const
    {
        return voxelCollisionMask.MakeCompoundShape();
    }
}