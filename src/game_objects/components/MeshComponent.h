﻿#pragma once
#include "game_objects/components/SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "rendering/mesh/MeshInstance.h"

namespace Vox
{
    class World;

    class MeshComponent : public SceneComponent
    {
    public:
        MeshComponent(Actor* parent);
        MeshComponent(Actor* parent, const std::string& meshName);
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

    protected:
        void OnTransformUpdated() override;
        
    private:

#ifdef EDITOR
        void Clicked(glm::ivec2 position);

        void Select() override;
#endif

        IMPLEMENT_OBJECT(MeshComponent)
        
        Ref<MeshInstance> mesh;
    };
}
