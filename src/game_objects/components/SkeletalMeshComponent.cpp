//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshComponent.h"

#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    SkeletalMeshComponent::SkeletalMeshComponent(Actor* parent)
        :SceneComponent(parent)
    {
        DEFAULT_DISPLAY_NAME()

        animationTime = 0.0f;
        animationIndex = 0;
        loop = true;
        playing = true;
    }

    SkeletalMeshComponent::SkeletalMeshComponent(Actor* parent, const std::string& meshName)
        :SkeletalMeshComponent(parent)
    {
        if (parent == nullptr || parent->GetWorld() == nullptr)
        {
            return;
        }

        mesh = GetParent()->GetWorld()->GetRenderer()->CreateSkeletalMeshInstance(meshName);

#ifdef EDITOR
        mesh->RegisterCallback([this](const glm::ivec2 position)
        {
            this->Clicked(position);
        });
#endif
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
        // @TODO: this is just a temp fix
        if (!mesh)
        {
            return;
        }

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
        ServiceLocator::GetEditorService()->GetEditor()->SelectObject(GetWeakThis());
    }

    void SkeletalMeshComponent::Select()
    {
        GetParent()->GetWorld()->GetRenderer()->AddMeshOutline(mesh);
    }
#endif
}
