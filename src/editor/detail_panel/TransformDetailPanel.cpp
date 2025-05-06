#include "TransformDetailPanel.h"

#include "Vec3DetailPanel.h"
#include "core/datatypes/Transform.h"

namespace Vox
{
    bool TransformDetailPanel::Draw(const char* label, Transform* transform, ImGuiInputTextFlags flags)
    {
        ImGui::Text(label);
        bool result = Vec3DetailPanel::Draw("Position", &transform->position, flags);
        result |= ImGui::InputFloat4("Rotation", &transform->rotation.x, "%.3f", flags);
        result |= Vec3DetailPanel::Draw("Scale", &transform->scale, flags);
        return result;
    }
}
