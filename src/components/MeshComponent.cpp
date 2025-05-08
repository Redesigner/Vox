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
        mesh->RegisterCallback([](glm::ivec2 test)
        {
           VoxLog(Display, Game, "Object clicked!"); 
        });
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
