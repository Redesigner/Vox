#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"

namespace Vox
{
    MeshComponent::MeshComponent()
    {
        DEFAULT_DISPLAY_NAME()
    }

    MeshComponent::MeshComponent(const std::string& meshName)
        :MeshComponent()
    {
        mesh = ServiceLocator::GetRenderer()->CreateMeshInstance(meshName);
    }

    void MeshComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);
    }
    
    void MeshComponent::OnTransformUpdated()
    {
        mesh->SetTransform(GetWorldTransform().GetMatrix());
    }

}
