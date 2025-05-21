//
// Created by steph on 5/14/2025.
//

#pragma once
#include <memory>
#include <string>

#include "Animation.h"
#include "SkeletalMeshInstance.h"
#include "core/datatypes/ObjectContainer.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    class SceneRenderer;
    class MaterialShader;
    class MeshShader;
    class PickShader;
    class SkeletalModel;

    class SkeletalMeshInstanceContainer
    {
    public:
        SkeletalMeshInstanceContainer(SceneRenderer* owner, size_t size, const std::shared_ptr<SkeletalModel>& mesh);

        bool LoadMesh(const std::string& filepath);

        void Render(MaterialShader* shader);

        void RenderInstance(const MeshShader* shader, const SkeletalMeshInstance& meshInstance) const;

        [[nodiscard]] const std::vector<Animation>& GetAnimations() const;

#ifdef EDITOR
        void Render(const PickShader* shader);
#endif

        Ref<SkeletalMeshInstance> CreateMeshInstance();

        [[nodiscard]] size_t GetInstanceCount() const;

        [[nodiscard]] SceneRenderer* GetOwner() const;

    private:
        SceneRenderer* owner;
        std::shared_ptr<SkeletalModel> mesh;
        ObjectContainer<SkeletalMeshInstance> meshInstances;
    };
}
