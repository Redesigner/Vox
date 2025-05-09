#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "rendering/shaders/pixel_shaders/PixelShader.h"

namespace Vox
{
    class DeferredShader : public PixelShader
    {
        struct UniformLocations
        {
            int position = -1;
            int normal = -1;
            int albedo = -1;
            int metallicRoughness = -1;
            int depth = -1;
            int viewPosition = -1;
        };

    public:
        DeferredShader();

        void SetCameraPosition(glm::vec3 position);

    private:
        UniformLocations uniformLocations;

        static inline std::string vertLocation = "assets/shaders/deferred.vert";
        static inline std::string fragLocation = "assets/shaders/deferred.frag";
    };
}
