#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Vox
{
    class DeferredShader;

    struct LightUniformLocations
    {
        LightUniformLocations();
        LightUniformLocations(DeferredShader* shader);

        int enabled;
        int type;
        int position;
        int target;
        int color;
        int strength;
    };

    struct Light
    {
        Light();
        Light(int enabled, int type, glm::vec3 position, glm::vec3 target, glm::vec4 color, float distance);

        int enabled;
        int type;
        glm::vec3 position;
        glm::vec3 target;
        glm::vec4 color;
        float strength;

        void UpdateLightValues(DeferredShader* shader, LightUniformLocations& uniformLocations);
    };
}