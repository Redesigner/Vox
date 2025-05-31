//
// Created by steph on 5/14/2025.
//

#pragma once
#include "SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "core/objects/Tickable.h"
#include "rendering/skeletal_mesh/SkeletalMeshInstance.h"

namespace Vox
{
    class World;
    struct SkeletalMeshInstance;

    class SkeletalMeshComponent : public SceneComponent, public Tickable
    {
    public:
        explicit SkeletalMeshComponent(const ObjectInitializer& objectInitializer);
        explicit SkeletalMeshComponent(const ObjectInitializer& objectInitializer, const std::string& meshName);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;

        void Tick(float DeltaTime) override;

    protected:
        void OnTransformUpdated() override;


    private:

#ifdef EDITOR
        void Clicked(glm::ivec2 position);

        void Select() override;
#endif

        IMPLEMENT_OBJECT(SkeletalMeshComponent)

        Ref<SkeletalMeshInstance> mesh;

        unsigned int animationIndex;
        float animationTime;
        bool loop;
        bool playing;
    };

}
