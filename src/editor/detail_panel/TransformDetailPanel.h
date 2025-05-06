#pragma once
#include <imgui.h>

namespace Vox
{
    struct Transform;

    class TransformDetailPanel
    {
    public:
        static bool Draw(const char* label, Transform* transform, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

    };
}
