#include "WorldOutline.h"

#include <imgui.h>

#include "core/objects/Object.h"
#include "core/objects/World.h"

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
            ImGui::TreePop();
        }
        result = ImGui::IsItemClicked();
        return result;
    }
}
