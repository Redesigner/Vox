//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshInstance.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/services/ServiceLocator.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
    SkeletalMeshInstance::SkeletalMeshInstance(SkeletalMeshInstanceContainer* meshOwner)
        :transform(glm::identity<glm::mat4x4>()), meshOwner(meshOwner)
    {
    }

    SkeletalMeshInstance::~SkeletalMeshInstance()
    {
#ifdef EDITOR
        if (pickId != 0)
        {
            ServiceLocator::GetRenderer()->GetPickContainer()->UnregisterCallback(pickId);
        }
#endif
    }

    SkeletalMeshInstance::SkeletalMeshInstance(SkeletalMeshInstance&& other) noexcept
        :transform(other.transform), meshOwner(other.meshOwner)
    {
#ifdef EDITOR
        pickId = other.pickId;
        other.pickId = 0;
#endif
    }

    void SkeletalMeshInstance::SetTransform(const glm::mat4x4& transformIn)
    {
        transform = transformIn;
    }

    glm::mat4x4 SkeletalMeshInstance::GetTransform() const
    {
        return transform;
    }

    SkeletalMeshInstanceContainer* SkeletalMeshInstance::GetMeshOwner() const
    {
        return meshOwner;
    }

#ifdef EDITOR
    void SkeletalMeshInstance::RegisterCallback(std::function<void(glm::ivec2)> callback)
    {
        pickId = ServiceLocator::GetRenderer()->GetPickContainer()->RegisterCallback(std::move(callback));
    }

    unsigned int SkeletalMeshInstance::GetPickId() const
    {
        return pickId;
    }
#endif
}
