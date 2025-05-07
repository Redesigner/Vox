#pragma once
#include <imgui.h>
#include <glm/vec3.hpp>

namespace Vox
{
    class Vec3DetailPanel
    {
    public:
        static bool Draw(const char* label, glm::vec3* vector, const char* format = "%.3f", ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

        static bool InputAxis(void* pData, ImColor color, const char* format);

        static ImColor xAxisColor;
        static ImColor yAxisColor;
        static ImColor zAxisColor;
    };
}
