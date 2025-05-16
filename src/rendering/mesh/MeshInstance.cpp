#include "MeshInstance.h"

#include <glm/ext/matrix_transform.hpp>

#include "core/logging/Logging.h"
#include "core/services/ServiceLocator.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
    MeshInstance::MeshInstance(MeshInstanceContainer* meshOwner, const std::vector<PBRMaterial>& materials)
        :visible(true), transform(glm::identity<glm::mat4x4>()), materials(materials), meshOwner(meshOwner)
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
        :visible(other.visible), transform(other.transform), materials(other.materials), meshOwner(other.meshOwner)
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

    void MeshInstance::SetMaterial(unsigned int index, const PBRMaterial& material)
    {
        // Consider making this an assert instead?
        if (index >= materials.size())
        {
            VoxLog(Warning, Rendering, "Cannot set materials, material at index '{}' is out of bounds.", index);
            return;
        }

        materials.at(index) = material;
    }

    glm::mat4x4 MeshInstance::GetTransform() const
    {
        return transform;
    }

    MeshInstanceContainer* MeshInstance::GetMeshOwner() const
    {
        return meshOwner;
    }

    const std::vector<PBRMaterial>& MeshInstance::GetMaterials() const
    {
        return materials;
    }

#ifdef EDITOR
    void MeshInstance::RegisterClickCallback(std::function<void(glm::ivec2)> callback)
    {
        pickId = ServiceLocator::GetRenderer()->GetPickContainer()->RegisterCallback(std::move(callback));
    }

    unsigned int MeshInstance::GetPickId() const
    {
        return pickId;
    }
#endif
}
