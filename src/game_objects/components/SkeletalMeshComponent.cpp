//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshComponent.h"

#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "rendering/Renderer.h"

namespace Vox
{
    SkeletalMeshComponent::SkeletalMeshComponent()
    {
        DEFAULT_DISPLAY_NAME()

        animationTime = 0.0f;
        animationIndex = 0;
    }

    SkeletalMeshComponent::SkeletalMeshComponent(const std::string& meshName)
        :SkeletalMeshComponent()
    {
        mesh = ServiceLocator::GetRenderer()->CreateSkeletalMeshInstance(meshName);

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
    }

    void SkeletalMeshComponent::PropertyChanged(const Property& property)
    {
        SceneComponent::PropertyChanged(property);

        if (property.GetValuePtr<float>(this) == &animationTime)
        {
            mesh->SetAnimationTime(animationTime);
            //animationTime = mesh->GetAnimationTime();
            return;
        }
        if (property.GetValuePtr<unsigned int>(this) == &animationIndex)
        {
            mesh->SetAnimationIndex(animationIndex);
            //animationIndex = mesh->GetAnimationIndex();
            return;
        }
    }

    void SkeletalMeshComponent::Tick(float DeltaTime)
    {
        animationTime += DeltaTime;
        mesh->SetAnimationTime(animationTime);
        animationTime = mesh->GetAnimationTime();
    }

    void SkeletalMeshComponent::OnTransformUpdated()
    {
        mesh->SetTransform(GetWorldTransform().GetMatrix());
    }

#ifdef EDITOR
    void SkeletalMeshComponent::Clicked(glm::ivec2 position)
    {
        ServiceLocator::GetRenderer()->ClearMeshOutlines();
        if (Editor* editor = ServiceLocator::GetEditorService()->GetEditor(); editor->GetSelectedObject() == this)
        {
            editor->SelectObject(nullptr);
        }
        else
        {
            editor->SelectObject(this);
            ServiceLocator::GetRenderer()->AddMeshOutline(mesh);
        }
    }
#endif
}
