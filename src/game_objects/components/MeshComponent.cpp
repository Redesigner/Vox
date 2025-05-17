#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/services/EditorService.h"
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

#ifdef EDITOR
        mesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            this->Clicked(position);
        });
#endif
    }

    void MeshComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);
    }
    
    void MeshComponent::OnTransformUpdated()
    {
        mesh->SetTransform(GetWorldTransform().GetMatrix());
    }

#ifdef EDITOR
    void MeshComponent::Clicked(glm::ivec2 position)
    {
        ServiceLocator::GetEditorService()->GetEditor()->SelectObject(this);
    }

    void MeshComponent::Select()
    {
        ServiceLocator::GetRenderer()->AddMeshOutline(mesh);
    }
#endif
}
