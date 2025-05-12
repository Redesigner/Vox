#include "MeshInstance.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/services/ServiceLocator.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
    MeshInstance::MeshInstance(MeshInstanceContainer* meshOwner)
        :transform(glm::identity<glm::mat4x4>()), meshOwner(meshOwner)
    {
    }

    MeshInstance::~MeshInstance()
    {
#ifdef EDITOR
        if (pickId != 0)
        {
            ServiceLocator::GetRenderer()->GetPickContainer()->UnregisterCallback(pickId);
        }
#endif
    }

    MeshInstance::MeshInstance(MeshInstance&& other) noexcept
        :transform(other.transform), meshOwner(other.meshOwner)
    {
#ifdef EDITOR
        pickId = other.pickId;
        other.pickId = 0;
#endif
    }

    void MeshInstance::SetTransform(glm::mat4x4 transformIn)
    {
        transform = transformIn;
    }

    glm::mat4x4 MeshInstance::GetTransform() const
    {
        return transform;
    }

    MeshInstanceContainer* MeshInstance::GetMeshOwner() const
    {
        return meshOwner;
    }

#ifdef EDITOR
    void MeshInstance::RegisterCallback(std::function<void(glm::ivec2)> callback)
    {
        pickId = ServiceLocator::GetRenderer()->GetPickContainer()->RegisterCallback(std::move(callback));
    }

    unsigned int MeshInstance::GetPickId() const
    {
        return pickId;
    }
#endif
}
