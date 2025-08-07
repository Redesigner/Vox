#include "DetailPanel.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <fmt/base.h>
#include <fmt/format.h>

#include "AssetPtrDetailPanel.h"
#include "TransformDetailPanel.h"
#include "Vec3DetailPanel.h"
#include "core/objects/ObjectClass.h"
#include "game_objects/actors/Actor.h"
#include "game_objects/components/Component.h"

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
        const auto component = dynamic_cast<Component*>(object);
        if (!component)
        {
            return;
        }

        TypedPropertyVariant defaultPropertyValue;

        // If this is a native component, the default properties come from the component's built-in properties, otherwise
        // the default properties come from the parent class's default object, after all the overrides have been applied
        if (component->native)
        {
            const auto owningActorClass = component->GetParent()->GetClass();
            const auto owningActorDefaultObject = dynamic_cast<const Actor*>(owningActorClass->GetDefaultObject());
            auto defaultComponent = owningActorDefaultObject->GetChildByName(component->GetDisplayName());
            defaultPropertyValue = property.GetTypedVariant(defaultComponent.get());
        }
        else
        {
            auto defaultComponent = dynamic_cast<const Component*>(component->GetClass()->GetDefaultObject());
            defaultPropertyValue = property.GetTypedVariant(defaultComponent);
        }
        property.SetValue(object, property.GetType(), defaultPropertyValue.value);
        object->PropertyChanged(property);
    }
}
