#include "TransformDetailPanel.h"

#include "Vec3DetailPanel.h"
#include "core/datatypes/Transform.h"

namespace Vox
{
    bool TransformDetailPanel::Draw(const char* label, Transform* transform, ImGuiInputTextFlags flags)
    {
        ImGui::Text(label);
        bool result = Vec3DetailPanel::Draw("Position", &transform->position, flags);

        glm::vec3 rotationEulerAxis = eulerAngles(transform->rotation);
        const bool rotationChanged = Vec3DetailPanel::Draw("Rotation", &rotationEulerAxis, flags);

        if (rotationChanged)
        {
            transform->rotation = glm::quat(rotationEulerAxis);
        }
        result |= rotationChanged;
        result |= Vec3DetailPanel::Draw("Scale", &transform->scale, flags);
        return result;
    }
}
