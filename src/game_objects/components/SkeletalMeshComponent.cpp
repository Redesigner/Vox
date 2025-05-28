//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshComponent.h"

#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "editor/EditorViewport.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    SkeletalMeshComponent::SkeletalMeshComponent(const ObjectInitializer& objectInitializer)
        :SceneComponent(objectInitializer)
    {
        DEFAULT_DISPLAY_NAME()

        animationTime = 0.0f;
        animationIndex = 0;
        loop = true;
        playing = true;
    }

    SkeletalMeshComponent::SkeletalMeshComponent(const ObjectInitializer& objectInitializer, const std::string& meshName)
        :SkeletalMeshComponent(objectInitializer)
    {
        if (objectInitializer.world)
        {
            mesh = objectInitializer.world->GetRenderer()->CreateSkeletalMeshInstance(meshName);
#ifdef EDITOR
            mesh->RegisterCallback([this](const glm::ivec2 position)
            {
                this->Clicked(position);
            });
#endif
        }

    }

    void SkeletalMeshComponent::BuildProperties(std::vector<Property>& propertiesInOut)
    {
        SceneComponent::BuildProperties(propertiesInOut);

        REGISTER_PROPERTY(float, animationTime);
        REGISTER_PROPERTY(unsigned int, animationIndex);
        REGISTER_PROPERTY(bool, loop);
        REGISTER_PROPERTY(bool, playing);
    }

    void SkeletalMeshComponent::PropertyChanged(const Property& property)
    {
        SceneComponent::PropertyChanged(property);

        if (property.GetValuePtr<float>(this) == &animationTime)
        {
            mesh->SetAnimationTime(animationTime);
            return;
        }
        if (property.GetValuePtr<unsigned int>(this) == &animationIndex)
        {
            mesh->SetAnimationIndex(animationIndex);
            return;
        }
        if (property.GetValuePtr<bool>(this) == &loop)
        {
            mesh->SetLooping(loop);
            return;
        }
    }

    void SkeletalMeshComponent::Tick(float DeltaTime)
    {
        if (playing)
        {
            animationTime += DeltaTime;
            mesh->SetAnimationTime(animationTime);
        }
        animationTime = mesh->GetAnimationTime();
        animationIndex = mesh->GetAnimationIndex();
    }

    void SkeletalMeshComponent::OnTransformUpdated()
    {
        mesh->SetTransform(GetWorldTransform().GetMatrix());
    }

#ifdef EDITOR
    void SkeletalMeshComponent::Clicked(glm::ivec2 position)
    {
        mesh->GetMeshOwner()->GetOwner()->viewport.lock()->OnObjectSelected(GetSharedThis());
    }

    void SkeletalMeshComponent::Select()
    {
        if (const auto* world = dynamic_cast<World*>(GetParent()->GetParent()))
        {
            world->GetRenderer()->AddMeshOutline(mesh);
        }
    }
#endif
}
