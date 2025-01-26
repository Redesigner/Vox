#include "VoxelBody.h"

namespace Vox
{
    VoxelBody::VoxelBody(unsigned int dimensions)
        :voxelCollisionMask(dimensions)
    {
    }

    void VoxelBody::CreateVoxel(glm::uvec3 position)
    {
        Octree::PhysicsVoxel solidVoxel = Octree::PhysicsVoxel(true);
        voxelCollisionMask.SetVoxel(position.x - 16, position.y - 16, position.z - 16, &solidVoxel);
    }

    void VoxelBody::EraseVoxel(glm::uvec3 position)
    {
        voxelCollisionMask.SetVoxel(position.x - 16, position.y - 16, position.z - 16, nullptr);
    }

    JPH::BodyID VoxelBody::GetBodyId() const
    {
        return bodyId;
    }

    void VoxelBody::SetBodyId(JPH::BodyID bodyIdIn)
    {
        bodyId = bodyIdIn;
    }

    JPH::Ref<JPH::StaticCompoundShapeSettings> VoxelBody::GetShapeSettings()
    {
        return voxelCollisionMask.MakeCompoundShape();
    }
}