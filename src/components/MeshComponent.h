#pragma once
#include "core/datatypes/Ref.h"
#include "core/objects/component/Component.h"

namespace Vox
{
    struct MeshInstance;

    class MeshComponent : public Component
    {
    public:
        MeshComponent();

        MeshComponent(const std::string& meshName);
        
        void BuildProperties(std::vector<Property>& propertiesInOut) override;

        void PropertyChanged(const Property& property) override;
        
    private:
        IMPLEMENT_OBJECT(MeshComponent)

        glm::vec3 position;

        Ref<MeshInstance> mesh;
    };
}
