//
// Created by steph on 5/14/2025.
//

#pragma once
#include <functional>
#include <glm/mat4x4.hpp>

namespace Vox
{
    class SkeletalMeshInstanceContainer;

    struct SkeletalMeshInstance
    {
        explicit SkeletalMeshInstance(SkeletalMeshInstanceContainer* meshOwner);
        ~SkeletalMeshInstance();

        SkeletalMeshInstance(SkeletalMeshInstance&& other) noexcept;

        void SetTransform(const glm::mat4x4& transformIn);

        [[nodiscard]] glm::mat4x4 GetTransform() const;

        [[nodiscard]] SkeletalMeshInstanceContainer* GetMeshOwner() const;

#ifdef EDITOR
        void RegisterCallback(std::function<void(glm::ivec2)> callback);

        [[nodiscard]] unsigned int GetPickId() const;
#endif

        private:
        glm::mat4x4 transform;

        SkeletalMeshInstanceContainer* meshOwner;

#ifdef EDITOR
        unsigned int pickId = 0;
#endif
    };
}
