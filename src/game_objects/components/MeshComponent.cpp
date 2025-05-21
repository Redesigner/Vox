#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    MeshComponent::MeshComponent(Actor* parent)
        :SceneComponent(parent)
    {
        DEFAULT_DISPLAY_NAME()
    }

    MeshComponent::MeshComponent(Actor* parent, const std::string& meshName)
        :MeshComponent(parent)
    {
        mesh = parent->GetWorld()->GetRenderer()->CreateMeshInstance(meshName);

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
        ServiceLocator::GetEditorService()->GetEditor()->SelectObject(GetWeakThis());
    }

    void MeshComponent::Select()
    {
        GetParent()->GetWorld()->GetRenderer()->AddMeshOutline(mesh);
    }
#endif
}
