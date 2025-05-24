#pragma once
#include <memory>

#include "PixelShader.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Camera;

    class SkyShader : public PixelShader
    {
    public:
        SkyShader();

        void SetCamera(const std::shared_ptr<Camera>& camera) const;

    private:
        static inline std::string vertLocation = "assets/shaders/sky.vert", fragLocation = "assets/shaders/sky.frag";

        int cameraWorldSpaceLocation = -1;
    };
} // Vox
