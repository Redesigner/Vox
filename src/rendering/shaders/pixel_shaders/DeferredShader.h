#pragma once

#include <array>
#include <string>

#include <glm/vec3.hpp>

#include "rendering/shaders/pixel_shaders/PixelShader.h"

namespace Vox
{
    struct Light;
}

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

        struct LightUniformLocations
        {
            int enabled = -1;
            int type = -1;
            int position = -1;
            int target = -1;
            int color = -1;
            int strength = -1;
        };

    public:
        DeferredShader();

        void SetCameraPosition(glm::vec3 position) const;

        void SetUniformLight(const Light& light, unsigned int index) const;

    private:
        static inline std::string vertLocation = "assets/shaders/deferred.vert";
        static inline std::string fragLocation = "assets/shaders/deferred.frag";

        static constexpr unsigned int lightsPerPass = 4;

        UniformLocations uniformLocations;
        std::array<LightUniformLocations, lightsPerPass> lightLocations;
    };
}
