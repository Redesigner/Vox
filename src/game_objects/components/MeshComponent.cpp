#include "MeshComponent.h"

#include "core/logging/Logging.h"
#include "core/objects/world/World.h"
#include "../actors/Actor.h"
#include "editor/EditorViewport.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    MeshComponent::MeshComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        DEFAULT_DISPLAY_NAME()

        meshAsset.type = AssetPtr::AssetType::Mesh;
    }

    MeshComponent::MeshComponent(const ObjectInitializer& objectInitializer, const std::string& meshName)
        :MeshComponent(objectInitializer)
    {
        meshAsset.path = meshName;
        if (objectInitializer.world)
        {
            mesh = objectInitializer.world->GetRenderer()->CreateMeshInstance(meshName);
        }

    }

    void MeshComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);

        REGISTER_PROPERTY(AssetPtr, meshAsset);
    }
    
    void MeshComponent::OnTransformUpdated()
    {
        if (!mesh)
        {
            return;
        }
        mesh->SetTransform(GetWorldTransform().GetMatrix());
    }

    void MeshComponent::PropertyChanged(const Property& property)
    {
        SceneComponent::PropertyChanged(property);

        UpdateMeshFromPath();
    }

    void MeshComponent::PostConstruct()
    {
        SceneComponent::PostConstruct();

        UpdateMeshFromPath();
    }

    void MeshComponent::UpdateMeshFromPath()
    {
        if (const Actor* parent = GetParent())
        {
            if (const World* world = parent->GetWorld())
            {
                mesh = world->GetRenderer()->CreateMeshInstance(meshAsset.path.string());
                if (mesh)
                {
                    mesh->SetTransform(GetWorldTransform().GetMatrix());
#ifdef EDITOR
                    RegisterClickCallback();
#endif
                }
            }
        }
    }

#ifdef EDITOR
    void MeshComponent::Clicked(glm::ivec2 position)
    {
        mesh->GetMeshOwner()->GetOwner()->viewport.lock()->OnObjectSelected(GetSharedThis());
    }

    void MeshComponent::Select()
    {
        if (mesh)
        {
            mesh->GetMeshOwner()->GetOwner()->AddMeshOutline(mesh);
        }
    }

    void MeshComponent::RegisterClickCallback()
    {
        mesh->RegisterClickCallback([this](const glm::ivec2 position)
        {
            this->Clicked(position);
        });
    }
#endif
}
