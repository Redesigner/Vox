//
// Created by steph on 5/14/2025.
//

#pragma once
#include "SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "rendering/skeletal_mesh/SkeletalMeshInstance.h"

namespace Vox
{
    struct SkeletalMeshInstance;

    class SkeletalMeshComponent : public SceneComponent
    {
    public:
        SkeletalMeshComponent();
        explicit SkeletalMeshComponent(const std::string& meshName);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

    protected:
        void OnTransformUpdated() override;

    private:

#ifdef EDITOR
        void Clicked(glm::ivec2 position);
#endif

        IMPLEMENT_OBJECT(SkeletalMeshComponent)

        Ref<SkeletalMeshInstance> mesh;
    };

}
