#include "MeshInstance.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/services/ServiceLocator.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
    MeshInstance::MeshInstance(MeshInstanceContainer* meshOwner)
        :meshOwner(meshOwner)
    {
        transform = glm::identity<glm::mat4x4>();
    }

    MeshInstance::~MeshInstance()
    {
#ifdef EDITOR
        ServiceLocator::GetRenderer()->GetPickContainer()->UnregisterCallback(pickId);
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
