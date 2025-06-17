#include "VoxelWorld.h"

#include "core/logging/Logging.h"
#include "core/objects/World.h"
#include "core/math/Formatting.h"
#include "core/math/Math.h"
#include "core/services/EditorService.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/SceneRenderer.h"
#include "rendering/camera/Camera.h"

namespace Vox
{
    VoxelWorld::VoxelWorld(World* world)
        :world(world)
    {
        raycastClickDelegate = ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](const int x, const int y)
            {
                CastScreenSpaceRay(x, y);
            });
    }

    VoxelWorld::~VoxelWorld() // NOLINT(*-use-equals-default)
    {
        ServiceLocator::GetInputService()->UnregisterMouseClickCallback(raycastClickDelegate);
    }

    std::optional<Voxel> VoxelWorld::GetVoxel(const glm::ivec3& position) const
    {
        auto [chunkPosition, voxelPosition] = GetChunkCoords(position);
        const auto chunkIterator = voxelChunks.find({chunkPosition.x, chunkPosition.y});
        if (chunkIterator == voxelChunks.end())
        {
            return std::nullopt;
        }

        return chunkIterator->second.GetVoxel({voxelPosition.x, position.y, voxelPosition.y});
    }

    void VoxelWorld::SetVoxel(const glm::ivec3& position, const Voxel& voxel)
    {
        auto [chunkPosition, voxelPosition] = GetChunkCoords(position);
        const auto chunkIterator = voxelChunks.find({chunkPosition.x, chunkPosition.y});
        if (chunkIterator == voxelChunks.end())
        {
            // If the chunk doesn't exist, allocate a new one
            // Default map can't hash ivec2, use std::pair here instead
            const auto [iterator, success] = voxelChunks.emplace(
                std::pair(chunkPosition.x, chunkPosition.y),
                VoxelChunk(chunkPosition, world->GetPhysicsServer().get(), world->GetRenderer().get())
            );
            VoxLog(Display, Game, "Allocating new voxel at coords '{}'", chunkPosition);
            iterator->second.SetVoxel({voxelPosition.x, position.y, voxelPosition.y}, voxel);

            if (std::ranges::find(modifiedChunks, iterator) == modifiedChunks.end())
            {
                modifiedChunks.push_back(iterator);
            }
            return;
        }

        chunkIterator->second.SetVoxel({voxelPosition.x, position.y, voxelPosition.y}, voxel);

        if (std::ranges::find(modifiedChunks, chunkIterator) == modifiedChunks.end())
        {
            modifiedChunks.push_back(chunkIterator);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void VoxelWorld::FinalizeUpdate()
    {
        for (const auto& modifiedChunk : modifiedChunks)
        {
            modifiedChunk->second.FinalizeUpdate();
        }
        modifiedChunks.clear();
    }

    std::pair<glm::ivec2, glm::ivec2> VoxelWorld::GetChunkCoords(const glm::ivec3& position)
    {
        auto [chunkX, voxelX] = std::div(position.x, VoxelChunk::chunkSize);
        auto [chunkY, voxelY] = std::div(position.z, VoxelChunk::chunkSize);
        if (voxelX < 0)
        {
            voxelX += VoxelChunk::chunkSize;
            --chunkX;
        }

        if (voxelY < 0)
        {
            voxelY += VoxelChunk::chunkSize;
            --chunkY;
        }

        return {{chunkX, chunkY}, {voxelX, voxelY}};
    }

    void VoxelWorld::CastScreenSpaceRay(int x, int y)
    {
            float xViewport, yViewport;
            std::shared_ptr<Camera> camera = world->GetRenderer()->GetCurrentCamera();
            if (!camera)
            {
                return;
            }

            if (ServiceLocator::GetEditorService()->GetEditor()->GetViewport()->GetClickViewportSpace(xViewport, yViewport, x, y))
            {
                const auto [start, end] = camera->GetWorldSpaceRay({xViewport, yViewport});
                JPH::Vec3 rayStart = Vec3From(start);
                JPH::Vec3 rayEnd = Vec3From(end);

                VoxLog(Display, Game, "Casting ray from '{}' to '{}'", rayStart, rayEnd);
                if (RayCastResultNormal raycastResult; world->GetPhysicsServer()->RayCast(rayStart, rayEnd - rayStart, raycastResult))
                {
                    // debugRenderer->DrawPersistentLine(raycastResult.impactPoint, raycastResult.impactPoint + raycastResult.impactNormal, JPH::Color::sBlue, 5.0f);
                    constexpr unsigned int gridSize = 1;
                    const JPH::Vec3 voxelEstimate = raycastResult.impactPoint - raycastResult.impactNormal / static_cast<float>(2 * gridSize);
                    JPH::Vec3 voxelPosition = {
                        static_cast<float>(FloorMultiple(voxelEstimate.GetX(), gridSize)),
                        static_cast<float>(FloorMultiple(voxelEstimate.GetY(), gridSize)),
                        static_cast<float>(FloorMultiple(voxelEstimate.GetZ(), gridSize))
                    };
                    VoxLog(Display, Game, "Clicked voxel at '{}'", voxelPosition);
                    voxelPosition += raycastResult.impactNormal;
                    const glm::ivec3 clickedVoxel = {voxelPosition.GetX() + 16, voxelPosition.GetY() + 16, voxelPosition.GetZ() + 16};
                    if (clickedVoxel.y >= 32 || clickedVoxel.y <= 0)
                    {
                        return;
                    }
                    Voxel newVoxel;
                    newVoxel.materialId = 1;
                    SetVoxel(clickedVoxel, newVoxel);
                    FinalizeUpdate();
                }
            }
    }
}
