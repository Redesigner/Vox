#include "TransformDetailPanel.h"

#include "Vec3DetailPanel.h"
#include "core/datatypes/Transform.h"

namespace Vox
{
    bool TransformDetailPanel::Draw(const char* label, Transform* transform, ImGuiInputTextFlags flags)
    {
        ImGui::SeparatorText(label);
        bool result = Vec3DetailPanel::Draw("Position", &transform->position, "%.3f", flags);

        glm::vec3 rotationEulerAxis = glm::degrees(transform->rotation);
        const bool rotationChanged = Vec3DetailPanel::Draw("Rotation", &rotationEulerAxis, "%.1f°", flags);

        if (rotationChanged)
        {
            transform->rotation = glm::radians(rotationEulerAxis);
        }
        
        result |= rotationChanged;
        result |= Vec3DetailPanel::Draw("Scale", &transform->scale, "%.3f", flags);
        return result;
    }
}
