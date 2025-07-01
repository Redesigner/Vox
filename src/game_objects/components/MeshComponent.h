#pragma once
#include "game_objects/components/SceneComponent.h"
#include "core/datatypes/Ref.h"
#include "rendering/mesh/MeshInstance.h"

namespace Vox
{
    class World;

    class MeshComponent : public SceneComponent
    {
    public:
        MeshComponent(const ObjectInitializer& objectInitializer);
        MeshComponent(const ObjectInitializer& objectInitializer, const std::string& meshName);

        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PostConstruct() override;

    protected:
        void OnTransformUpdated() override;

        void PropertyChanged(const Property& property) override;

    private:
        void UpdateMeshFromPath();

#ifdef EDITOR
        void Clicked(glm::ivec2 position);

        void Select() override;

        void RegisterClickCallback();
#endif

        IMPLEMENT_OBJECT(MeshComponent, SceneComponent)
        
        Ref<MeshInstance> mesh;

        AssetPtr meshAsset;
    };
}
