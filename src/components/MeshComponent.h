#pragma once
#include "components/SceneComponent.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    struct MeshInstance;

    class MeshComponent : public SceneComponent
    {
    public:
        MeshComponent();

        MeshComponent(const std::string& meshName);
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

    protected:
        void OnTransformUpdated() override;
        
    private:
        IMPLEMENT_OBJECT(MeshComponent)
        
        Ref<MeshInstance> mesh;
    };
}
