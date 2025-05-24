#pragma once
#include <memory>

#include "PixelShader.h"

namespace Vox
{
    class Camera;

    class DebugShader : public PixelShader
    {
    public:
        DebugShader(const std::string& vertLocation, const std::string& fragLocation);

        void SetCamera(const std::shared_ptr<Camera>& camera) const;

    private:
        int viewProjectionMatrixLocation = -1;
    };
} // Vox
