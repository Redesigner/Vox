#include "DetailPanel.h"

#include <imgui.h>

namespace Vox
{
    void DetailPanel::Draw(Object* object)
    {
        if (ImGui::Begin("Object", 0))
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
                std::string string = *property.GetValuePtr<std::string>(object);
                size_t size = string.size();
                ImGui::InputText(property.GetName().c_str(), string.data(), size);
                break;
            }
        }
    }
}
