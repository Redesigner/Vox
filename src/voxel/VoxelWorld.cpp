#include "VoxelWorld.h"

#include "Octree.h"
#include "core/logging/Logging.h"
#include "core/math/Formatting.h"
#include "core/math/Math.h"
#include "core/objects/world/World.h"
#include "core/services/EditorService.h"
#include "core/services/FileIOService.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/SceneRenderer.h"
#include "rendering/camera/Camera.h"

namespace Vox
{
    VoxelWorld::VoxelWorld(const World* world)
        :world(world)
    {
    }

    VoxelWorld::VoxelWorld(const World* world, const std::string& filename)
        :VoxelWorld(world)
    {
        const std::string data = ServiceLocator::GetFileIoService()->LoadFile(fmt::format("worlds/{}.vox", filename));

        size_t cursorPosition = data.find('\n');
        while (cursorPosition != std::string::npos)
        {
            VoxelChunk newChunk(data.substr(0, data.find('\n')), world);
            voxelChunks.emplace(newChunk.GetChunkLocation(), std::move(newChunk));
            cursorPosition = data.find('\n', cursorPosition + 1);
        }
    }

    VoxelWorld::~VoxelWorld()
    = default;

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
            const auto [iterator, success] = voxelChunks.emplace(
                chunkPosition,
                VoxelChunk(chunkPosition, world)
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

    void VoxelWorld::SaveToFile(const std::string& filename) const
    {
        ServiceLocator::GetFileIoService()->WriteToFile(fmt::format("worlds/{}.vox", filename), WriteString());
    }

    std::optional<VoxelRaycastResult> VoxelWorld::CastScreenSpaceRay(const glm::ivec2& screenSpace) const
    {
        float xViewport, yViewport;
        std::shared_ptr<Camera> camera = world->GetRenderer()->GetCurrentCamera();
        if (!camera)
        {
            return std::nullopt;
        }

        if (ServiceLocator::GetEditorService()->GetEditor()->GetViewport()->GetClickViewportSpace(xViewport, yViewport, screenSpace.x, screenSpace.y))
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
                glm::ivec3 voxelPosition = {
                    FloorMultiple(voxelEstimate.GetX(), gridSize),
                    FloorMultiple(voxelEstimate.GetY(), gridSize),
                    FloorMultiple(voxelEstimate.GetZ(), gridSize)
                };
                VoxLog(Display, Game, "Clicked voxel at '{}'", voxelPosition);

                const glm::ivec3 clickedVoxel = voxelPosition + 16;
                if (clickedVoxel.y >= 32 || clickedVoxel.y <= 0)
                {
                    return std::nullopt;
                }
                const glm::ivec3 voxelNormal = {
                    std::round(raycastResult.impactNormal.GetX()),
                    std::round(raycastResult.impactNormal.GetY()),
                    std::round(raycastResult.impactNormal.GetZ())
                };

                return VoxelRaycastResult(clickedVoxel, voxelNormal * static_cast<int>(gridSize));
            }
        }
        return std::nullopt;
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

    std::string VoxelWorld::WriteString() const
    {
        std::string result;
        for (const auto& chunk : voxelChunks | std::views::values)
        {
            result.append(chunk.WriteString());
            result += "\n";
        }
        return result;
    }
}
