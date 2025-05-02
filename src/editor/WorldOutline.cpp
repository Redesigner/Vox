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
                if (DrawObject(object))
                {
                    currentlySelectedObject = object;
                }
            }
        }
        ImGui::End();
    }

    Object* WorldOutline::GetSelectedObject()
    {
        return currentlySelectedObject;
    }

    bool WorldOutline::DrawObject(Object* object)
    {
        bool result = false;
        ImGuiTreeNodeFlags flags = object == currentlySelectedObject ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
        if (ImGui::TreeNodeEx(fmt::format("{} {}", object->GetClassDisplayName(), object->GetDisplayName()).c_str(), flags))
        {
            if (Actor* actor = dynamic_cast<Actor*>(object))
            {
                for (const std::unique_ptr<Component>& component : actor->GetComponents())
                {
                    if (ImGui::Selectable(fmt::format("\t{}", component->GetDisplayName()).c_str()))
                    {
                        currentlySelectedObject = component.get();
                    }
                }
            }
            ImGui::TreePop();
        }
        result = ImGui::IsItemClicked();
        return result;
    }
}
