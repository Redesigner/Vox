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
    class MaterialShader;
    class MeshShader;
    class PickShader;
    class SkeletalModel;

    class SkeletalMeshInstanceContainer
    {
    public:
        explicit SkeletalMeshInstanceContainer(size_t size);

        bool LoadMesh(const std::string& filepath);

        void Render(MaterialShader* shader);

        void RenderInstance(const MeshShader* shader, const SkeletalMeshInstance& meshInstance) const;

        [[nodiscard]] const std::vector<Animation>& GetAnimations() const;

#ifdef EDITOR
        void Render(const PickShader* shader);
#endif

        Ref<SkeletalMeshInstance> CreateMeshInstance();

        [[nodiscard]] size_t GetInstanceCount() const;

    private:
        std::unique_ptr<SkeletalModel> model;
        ObjectContainer<SkeletalMeshInstance> meshInstances;
    };
}
