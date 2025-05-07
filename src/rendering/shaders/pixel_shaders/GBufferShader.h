#pragma once
#include "PixelShader.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    struct PBRMaterial;
}

namespace Vox
{
    class Camera;

    class GBufferShader : public PixelShader
    {
        struct UniformLocations
        {
            int modelMatrix;
            int viewMatrix;
            int projectionMatrix;
            int roughness;
            int albedo;
        };
        
    public:
        GBufferShader();

        void SetCamera(Ref<Camera> camera) const;
        void SetMaterial(const PBRMaterial& material) const;
        void SetModelMatrix(const glm::mat4x4& matrix) const;

    private:
        UniformLocations uniformLocations;

        const std::string vertLocation = "assets/shaders/gBuffer.vert";
        const std::string fragLocation = "assets/shaders/gBuffer.frag";
    };
}
