#include "WorldOutline.h"

#include <imgui.h>

#include "core/objects/Object.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/objects/component/Component.h"

namespace Vox
{
    void WorldOutline::Draw(World* world)
    {
        if (ImGui::Begin("World Outline"))
        {
            for (Object* object : world->GetObjects())
            {
                DrawObject(object);
            }
        }
        ImGui::End();
    }

    Object* WorldOutline::GetSelectedObject()
    {
        return currentlySelectedObject;
    }

    void WorldOutline::SetSelectedObject(Object* object)
    {
        currentlySelectedObject = object;
    }

    void WorldOutline::DrawObject(Object* object)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (object == currentlySelectedObject)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        const bool itemExpanded = ImGui::TreeNodeEx(fmt::format("{} {}", object->GetClassDisplayName(), object->GetDisplayName()).c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            currentlySelectedObject = object;
        }
        if (itemExpanded)
        {
            if (const Actor* actor = dynamic_cast<Actor*>(object))
            {
                for (const auto& sceneComponent : actor->GetAttachedComponents())
                {
                    if (ImGui::Selectable(fmt::format("\t{}", sceneComponent->GetDisplayName()).c_str(), sceneComponent.get() == currentlySelectedObject))
                    {
                        currentlySelectedObject = sceneComponent.get();
                    }
                }
                
                for (const std::shared_ptr<Component>& component : actor->GetComponents())
                {
                    if (ImGui::Selectable(
                        fmt::format("\t{}", component->GetDisplayName()).c_str(),
                        component.get() == currentlySelectedObject))
                    {
                        currentlySelectedObject = component.get();
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}
