#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    MeshComponent::MeshComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        DEFAULT_DISPLAY_NAME()
    }

    MeshComponent::MeshComponent(const ObjectInitializer& objectInitializer, const std::string& meshName)
        :MeshComponent(objectInitializer)
    {
        if (objectInitializer.world)
        {
            mesh = objectInitializer.world->GetRenderer()->CreateMeshInstance(meshName);
#ifdef EDITOR
            mesh->RegisterClickCallback([this](const glm::ivec2 position)
            {
                this->Clicked(position);
            });
#endif
        }

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
        mesh->GetMeshOwner()->GetOwner()->viewport.lock()->OnObjectSelected(GetWeakThis().lock());
    }

    void MeshComponent::Select()
    {
        GetParent()->GetWorld()->GetRenderer()->AddMeshOutline(mesh);
    }
#endif
}
