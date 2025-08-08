#include "Light.h"

#include <fmt/format.h>

#include "rendering/shaders/pixel_shaders/DeferredShader.h"

namespace Vox
{
    Light::Light()
        :enabled(false), type(0), position(), target(), color(), strength(0.0f)
    {
    }

    Light::Light(int enabled, int type, glm::vec3 position, glm::vec3 target, glm::vec4 color, float distance)
        :enabled(enabled), type(type), position(position), target(target), color(color), strength(distance)
    {
    }
}