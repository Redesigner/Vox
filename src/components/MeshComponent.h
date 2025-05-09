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

        explicit MeshComponent(const std::string& meshName);
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

    protected:
        void OnTransformUpdated() override;
        
    private:

#ifdef EDITOR
        void Clicked(glm::ivec2 position);
#endif

        IMPLEMENT_OBJECT(MeshComponent)
        
        Ref<MeshInstance> mesh;
    };
}
