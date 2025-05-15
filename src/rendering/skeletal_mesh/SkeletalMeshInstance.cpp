//
// Created by steph on 5/14/2025.
//

#include "SkeletalMeshInstance.h"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

#include "core/logging/Logging.h"
#include "core/services/ServiceLocator.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"

namespace Vox
{
    SkeletalMeshInstance::SkeletalMeshInstance(SkeletalMeshInstanceContainer* meshOwner)
        :transform(glm::identity<glm::mat4x4>()), meshOwner(meshOwner), animationIndex(0), currentAnimationTime(0.0f), loopAnimation(true)
    {
    }

    SkeletalMeshInstance::~SkeletalMeshInstance()
    {
#ifdef EDITOR
        if (pickId != 0)
        {
            ServiceLocator::GetRenderer()->GetPickContainer()->UnregisterCallback(pickId);
        }
#endif
    }

    SkeletalMeshInstance::SkeletalMeshInstance(SkeletalMeshInstance&& other) noexcept
        :transform(other.transform), meshOwner(other.meshOwner), animationIndex(other.animationIndex),
        loopAnimation(other.loopAnimation), currentAnimationTime(other.currentAnimationTime)
    {
#ifdef EDITOR
        pickId = other.pickId;
        other.pickId = 0;
#endif
    }

    void SkeletalMeshInstance::SetTransform(const glm::mat4x4& transformIn)
    {
        transform = transformIn;
    }

    glm::mat4x4 SkeletalMeshInstance::GetTransform() const
    {
        return transform;
    }

    SkeletalMeshInstanceContainer* SkeletalMeshInstance::GetMeshOwner() const
    {
        return meshOwner;
    }

#ifdef EDITOR
    void SkeletalMeshInstance::RegisterCallback(std::function<void(glm::ivec2)> callback)
    {
        pickId = ServiceLocator::GetRenderer()->GetPickContainer()->RegisterCallback(std::move(callback));
    }

    unsigned int SkeletalMeshInstance::GetPickId() const
    {
        return pickId;
    }
#endif

    void SkeletalMeshInstance::SetAnimationTime(float time)
    {
        if (animationIndex >= meshOwner->GetAnimations().size())
        {
            VoxLog(Error, Rendering, "Attempted to set animationIndex to an out-of-bounds value.");
            return;
        }
        const Animation& currentAnimation = meshOwner->GetAnimations().at(animationIndex);
        float duration = currentAnimation.GetDuration();

        if (duration == 0.0f)
        {
            return;
        }

        if (loopAnimation)
        {
            if (time >= duration)
            {
                time = std::fmod(time, duration);
            }
        }
        currentAnimationTime = std::clamp(time, 0.0f, duration);
    }

    void SkeletalMeshInstance::SetAnimationIndex(const unsigned int index)
    {
        if (index >= meshOwner->GetAnimations().size())
        {
            VoxLog(Error, Rendering, "Attempted to set animationIndex to an out-of-bounds value.");
            return;
        }

        if (animationIndex != index)
        {
            currentAnimationTime = std::min(currentAnimationTime, meshOwner->GetAnimations().at(index).GetDuration());
            animationIndex = index;
        }
    }

    void SkeletalMeshInstance::SetLooping(bool looping)
    {
        loopAnimation = looping;
    }

    float SkeletalMeshInstance::GetAnimationTime() const
    {
        return currentAnimationTime;
    }

    unsigned int SkeletalMeshInstance::GetAnimationIndex() const
    {
        return animationIndex;
    }

    bool SkeletalMeshInstance::GetLooping() const
    {
        return loopAnimation;
    }
}
