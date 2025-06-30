#include "DetailPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <fmt/base.h>
#include <fmt/format.h>

#include "AssetPtrDetailPanel.h"
#include "TransformDetailPanel.h"
#include "Vec3DetailPanel.h"
#include "core/objects/ObjectClass.h"

namespace Vox
{
    void DetailPanel::Draw(Object* object)
    {
        assert(object);
        // ReSharper disable once CppDFANullDereference
        if (ImGui::BeginChild(fmt::format("{}###DetailPanel", object->GetDisplayName()).c_str()))
        {
            for (const Property& property : object->GetProperties())
            {
                DrawProperty(object, property);
                ImGui::SameLine();
                if (ImGui::Button(std::format("R###{}", property.GetFriendlyName()).c_str()))
                {
                    ResetProperty(object, property);
                }
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
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
            propertyChanged = ImGui::InputText(property.GetFriendlyName().c_str(), &string, ImGuiInputTextFlags_None);
            break;
        }

        case PropertyType::_vec3:
        {
            auto* vector = property.GetValuePtr<glm::vec3>(object);
            propertyChanged = Vec3DetailPanel::Draw(property.GetFriendlyName().c_str(), vector);
            break;
        }

        case PropertyType::_transform:
        {
            auto* transform = property.GetValuePtr<Transform>(object);
            propertyChanged = TransformDetailPanel::Draw(property.GetFriendlyName().c_str(), transform);
            break;
        }

        case PropertyType::_uint:
        {
            propertyChanged = ImGui::InputScalar(property.GetFriendlyName().c_str(), ImGuiDataType_U32, property.GetValuePtr<unsigned int>(object));
            break;
        }

        case PropertyType::_assetPtr:
        {
            propertyChanged = AssetPtrDetailPanel::Draw(property.GetValueChecked<AssetPtr>(object), property.GetName());
            break;
        }

        default:
            break;
        }

        if (propertyChanged)
        {
            object->PropertyChanged(property);
        }
    }

    void DetailPanel::ResetProperty(Object* object, const Property& property)
    {
        /*const auto& root = object->GetRoot();
        const auto& rootClass = root->native ? root->GetClass()->GetParentClass() : root->GetClass();
        const Object* defaultRootObject = rootClass->GetDefaultObject();
        const std::vector<std::string> objectPath = object->GetRootPath();
        std::shared_ptr<Object> defaultObject = defaultRootObject->GetChildByPath(objectPath);
        if (!defaultObject)
        {
            property.SetValue(object, property.GetType(), property.GetTypedVariant(object->GetClass()->GetDefaultObject()).value);
            return;
        }
        TypedPropertyVariant defaultValue = property.GetTypedVariant(defaultObject.get());
        property.SetValue(object, property.GetType(), defaultValue.value);
        object->PropertyChanged(property);*/
    }
}
