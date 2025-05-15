﻿#include "DetailPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <fmt/base.h>
#include <fmt/format.h>

#include "TransformDetailPanel.h"
#include "Vec3DetailPanel.h"

namespace Vox
{
    void DetailPanel::Draw(Object* object)
    {
        assert(object);
        if (ImGui::Begin(fmt::format("{}###DetailPanel", object->GetDisplayName()).c_str(), 0))
        {
            for (const Property& property : object->GetProperties())
            {
                DrawProperty(object, property);
            }
        }
        ImGui::End();
    }

    void DetailPanel::DrawProperty(Object* object, const Property& property)
    {
        bool propertyChanged = false;
        switch (property.GetType())
        {
        case PropertyType::_bool:
            {
                propertyChanged = ImGui::Checkbox(property.GetFriendlyName().c_str(), property.GetValuePtr<bool>(object));
                break;
            }
        case PropertyType::_float:
            {
                propertyChanged = ImGui::InputFloat(property.GetFriendlyName().c_str(), property.GetValuePtr<float>(object));
                break;
            }
        case PropertyType::_string:
            {
                std::string& string = *property.GetValuePtr<std::string>(object);
                size_t size = string.size();
                propertyChanged = ImGui::InputText(property.GetFriendlyName().c_str(), &string, ImGuiInputTextFlags_None);
                break;
            }
        case PropertyType::_vec3:
            {
                glm::vec3* vector = property.GetValuePtr<glm::vec3>(object);
                propertyChanged = Vec3DetailPanel::Draw(property.GetFriendlyName().c_str(), vector);
                break;
            }
        case PropertyType::_transform:
            {
                Transform* transform = property.GetValuePtr<Transform>(object);
                propertyChanged = TransformDetailPanel::Draw(property.GetFriendlyName().c_str(), transform);
                break;
            }
        case PropertyType::_uint:
            {
                propertyChanged = ImGui::InputScalar(property.GetFriendlyName().c_str(), ImGuiDataType_U32, property.GetValuePtr<unsigned int>(object));
                break;
            }
        }

        if (propertyChanged)
        {
            object->PropertyChanged(property);
        }
    }
}
