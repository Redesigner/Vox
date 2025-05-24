#pragma once
#include <memory>

#include "core/datatypes/Ref.h"
#include "rendering/shaders/pixel_shaders/PixelShader.h"

namespace Vox
{
    class Camera;
    class MeshShader : public PixelShader
    {
    public:
        MeshShader(std::string vertLocation, std::string fragLocation);

        void SetCamera(const std::shared_ptr<Camera>& camera) const;
        void SetModelMatrix(const glm::mat4x4& matrix) const;

    private:
        struct UniformLocations
        {
            int modelMatrix;
            int viewMatrix;
            int projectionMatrix;
        };

        UniformLocations uniformLocations;
    };
}
