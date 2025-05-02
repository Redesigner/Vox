#include "DetailPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <fmt/base.h>
#include <fmt/format.h>

#include "Vec3DetailPanel.h"

namespace Vox
{
    void DetailPanel::Draw(Object* object)
    {
        assert(object);
        if (ImGui::Begin(fmt::format("{}###DetailPanel", object->GetClassDisplayName()).c_str(), 0))
        {
            for (Property& property : object->GetProperties())
            {
                DrawProperty(object, property);
            }
        }
        ImGui::End();
    }

    void DetailPanel::DrawProperty(Object* object, Property& property)
    {
        switch (property.GetType())
        {
        case PropertyType::_bool:
            {
                ImGui::Checkbox(property.GetName().c_str(), property.GetValuePtr<bool>(object));
                break;
            }
        case PropertyType::_float:
            {
                ImGui::InputFloat(property.GetName().c_str(), property.GetValuePtr<float>(object));
                break;
            }
        case PropertyType::_string:
            {
                std::string& string = *property.GetValuePtr<std::string>(object);
                size_t size = string.size();
                ImGui::InputText(property.GetName().c_str(), &string, ImGuiInputTextFlags_None);
                break;
            }
        case PropertyType::_vec3:
            {
                glm::vec3* vector = property.GetValuePtr<glm::vec3>(object);
                Vec3DetailPanel::Draw(property.GetName().c_str(), vector);
            }
        }
    }
}
