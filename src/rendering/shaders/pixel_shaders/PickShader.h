#pragma once
#include "PixelShader.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Camera;

    class PickShader : public PixelShader
    {
        struct UniformLocations
        {
            int modelMatrix;
            int viewMatrix;
            int projectionMatrix;
            int objectId;
        };
        
    public:
        PickShader();

        void SetCamera(Ref<Camera> camera) const;
        void SetObjectId(int objectId) const;
        void SetModelMatrix(const glm::mat4x4& matrix) const;

    private:
        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/pickBuffer.vert";
        static inline std::string fragLocation = "assets/shaders/pickBuffer.frag";
    };
}
