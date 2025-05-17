#include "WorldOutline.h"

#include <imgui.h>

#include "core/objects/Object.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/objects/component/Component.h"
#include "rendering/Renderer.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    WorldOutline::WorldOutline()
    {
        gizmo = nullptr;
    }

    void WorldOutline::InitializeGizmos()
    {
        gizmo = std::make_unique<Gizmo>();
    }

    void WorldOutline::Draw(const World* world)
    {
        if (ImGui::Begin("World Outline"))
        {
            for (Object* object : world->GetObjects())
            {
                DrawObject(object);
            }
        }
        ImGui::End();
        UpdateGizmos();
    }

    void WorldOutline::UpdateGizmos() const
    {
        if (gizmo)
        {
            if (auto* component = dynamic_cast<SceneComponent*>(currentlySelectedObject))
            {
                gizmo->SetTransform(component->GetWorldTransform());
                gizmo->Update();
                Transform newTransform = Transform(glm::inverse(component->GetParent()->GetTransform().GetMatrix()) * gizmo->GetTransform().GetMatrix());
                component->SetTransform(newTransform);
            }

            if (auto* actor = dynamic_cast<Actor*>(currentlySelectedObject))
            {
                gizmo->SetTransform(actor->GetTransform());
                gizmo->Update();
                actor->SetTransform(gizmo->GetTransform());
            }
        }
    }

    Object* WorldOutline::GetSelectedObject() const
    {
        return currentlySelectedObject;
    }

    void WorldOutline::SetSelectedObject(Object* object)
    {
        ServiceLocator::GetRenderer()->ClearMeshOutlines();
        if (object == currentlySelectedObject)
        {
            object = nullptr;
        }

        currentlySelectedObject = object;
        if (!object)
        {
            gizmo->SetVisible(false);
            return;
        }

        if (const auto component = dynamic_cast<SceneComponent*>(object))
        {
            gizmo->SetVisible(true);
            component->Select();
            gizmo->SetTransform(component->GetWorldTransform());
        }

        if (const auto actor = dynamic_cast<Actor*>(object))
        {
            gizmo->SetVisible(true);
            actor->Select();
            gizmo->SetTransform(actor->GetTransform());
        }
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
            SetSelectedObject(object);
        }
        if (itemExpanded)
        {
            if (const Actor* actor = dynamic_cast<Actor*>(object))
            {
                for (const auto& sceneComponent : actor->GetAttachedComponents())
                {
                    if (ImGui::Selectable(fmt::format("\t{}", sceneComponent->GetDisplayName()).c_str(), sceneComponent.get() == currentlySelectedObject))
                    {
                        SetSelectedObject(sceneComponent.get());
                    }
                }
                
                for (const std::shared_ptr<Component>& component : actor->GetComponents())
                {
                    if (ImGui::Selectable(
                        fmt::format("\t{}", component->GetDisplayName()).c_str(),
                        component.get() == currentlySelectedObject))
                    {
                        SetSelectedObject(component.get());
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}
