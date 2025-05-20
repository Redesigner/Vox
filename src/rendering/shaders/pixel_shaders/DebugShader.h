#pragma once
#include "PixelShader.h"
#include "core/datatypes/Ref.h"

namespace Vox
{
    class Camera;

    class DebugShader : public PixelShader
    {
    public:
        DebugShader(const std::string& vertLocation, const std::string& fragLocation);

        void SetCamera(const Ref<Camera>& camera) const;

    private:
        int viewProjectionMatrixLocation = -1;
    };
} // Vox
