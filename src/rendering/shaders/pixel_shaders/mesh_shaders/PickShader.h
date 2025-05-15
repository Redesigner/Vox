#pragma once
#include "rendering/shaders/pixel_shaders/mesh_shaders/MeshShader.h"

namespace Vox
{
    class Camera;

    class PickShader : public MeshShader
    {
        struct UniformLocations
        {
            int objectId = -1;
        };
        
    public:
        PickShader(const std::string& vertLocation, const std::string& fragLocation);

        void SetObjectId(unsigned int objectId) const;

    private:
        UniformLocations uniformLocations;
    };
}
