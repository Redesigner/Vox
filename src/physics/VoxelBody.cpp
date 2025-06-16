#include "VoxelBody.h"

#include "voxel/VoxelChunk.h"

namespace Vox
{
    VoxelBody::VoxelBody()
        :chunkPosition({0, 0})
    {
        voxelCollisionMask = std::make_unique<Octree::CollisionNode>(VoxelChunk::chunkSize);
    }

    VoxelBody::~VoxelBody()
    = default;

    VoxelBody::VoxelBody(VoxelBody&& other) noexcept
        :chunkPosition(other.chunkPosition)
    {
        bodyId = other.bodyId;
        voxelCollisionMask = std::move(other.voxelCollisionMask);
    }

    void VoxelBody::CreateVoxel(const glm::uvec3 position)
    {
        auto solidVoxel = Octree::PhysicsVoxel(true);
        voxelCollisionMask->SetVoxel(
            static_cast<int>(position.x - VoxelChunk::chunkHalfSize),
            static_cast<int>(position.y - VoxelChunk::chunkHalfSize),
            static_cast<int>(position.z - VoxelChunk::chunkHalfSize),
            solidVoxel);
    }

    void VoxelBody::EraseVoxel(const glm::uvec3 position)
    {
        voxelCollisionMask->SetVoxel(
            static_cast<int>(position.x - VoxelChunk::chunkHalfSize),
            static_cast<int>(position.y - VoxelChunk::chunkHalfSize),
            static_cast<int>(position.z - VoxelChunk::chunkHalfSize),
            Octree::PhysicsVoxel());
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
        return voxelCollisionMask->MakeCompoundShape();
    }

    const glm::ivec2& VoxelBody::GetChunkPosition() const
    {
        return chunkPosition;
    }
}
