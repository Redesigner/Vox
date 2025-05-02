#pragma once
#include <imgui.h>
#include <glm/vec3.hpp>

namespace Vox
{
    class Vec3DetailPanel
    {
    public:
        static bool Draw(const char* label, glm::vec3* vector, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

        static bool InputAxis(void* pData, ImColor color);

        static ImColor xAxisColor;
        static ImColor yAxisColor;
        static ImColor zAxisColor;
    };
}
