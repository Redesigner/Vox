#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"

namespace Vox
{
    MeshComponent::MeshComponent()
    {
        DEFAULT_DISPLAY_NAME()

        position = {0.0f, 0.0f, 0.0f};
    }

    MeshComponent::MeshComponent(const std::string& meshName)
        :MeshComponent()
    {
        mesh = ServiceLocator::GetRenderer()->CreateMeshInstance(meshName);
    }

    void MeshComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        Component::BuildProperties(propertiesInOut);

        REGISTER_PROPERTY(glm::vec3, position)
    }

    void MeshComponent::PropertyChanged(const Property& property)
    {
        if (property.GetName() == "position")
        {
            if (!mesh)
            {
                VoxLog(Error, Game, "MeshComponent failed to update position! Mesh was invalid.");
            }
            Transform meshTransform = mesh->GetTransform();
            meshTransform.position = property.GetValueChecked<glm::vec3>(this);
            mesh->SetTransform(meshTransform.GetMatrix());
        }
    }
}
