#pragma once
#include "rendering/shaders/pixel_shaders/mesh_shaders/MeshShader.h"

namespace Vox
{
    class Camera;
    struct PBRMaterial;

    class GBufferShader : public MeshShader
    {
        struct UniformLocations
        {
            int roughness = -1;
            int albedo = -1;
        };
        
    public:
        GBufferShader();

        void SetMaterial(const PBRMaterial& material) const;

    private:
        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/gBuffer.vert";
        static inline std::string fragLocation = "assets/shaders/gBuffer.frag";
    };
}
