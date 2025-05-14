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
        PickShader();

        void SetObjectId(unsigned int objectId) const;

    private:
        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/pickBuffer.vert";
        static inline std::string fragLocation = "assets/shaders/pickBuffer.frag";
    };
}
