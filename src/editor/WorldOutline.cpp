#include "WorldOutline.h"

#include <imgui.h>

#include "Editor.h"
#include "core/objects/Object.h"
#include "core/objects/World.h"
#include "core/objects/actor/Actor.h"
#include "core/objects/component/Component.h"
#include "core/services/InputService.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    WorldOutline::WorldOutline()
    {
        tempWorld = nullptr;
        gizmo = nullptr;

        ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_DELETE, [this](bool pressed)
        {
            if (pressed)
            {
                tempWorld->DestroyObject(currentlySelectedObject.lock());
            }
        });
    }

    void WorldOutline::InitializeGizmos(World* world)
    {
        gizmo = std::make_unique<Gizmo>(world->GetRenderer().get());
    }

    void WorldOutline::Draw(World* world)
    {
        tempWorld = world;
        ImGui::PushFont(Editor::GetFont_GitLab18());
        if (ImGui::BeginTabBar("WorldOutlineTabs"))
        {
            if (ImGui::BeginTabItem("World Outline"))
            {
                ImGui::PushFont(Editor::GetFont_GitLab14());
                ImGui::PushStyleColor(ImGuiCol_ChildBg, Editor::lightBgColor);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::BeginChild("WorldOutlinePanel", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding);
                for (const auto& object : world->GetObjects())
                {
                    DrawObject(object);
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopFont();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::PopFont();
        UpdateGizmos();
    }

    void WorldOutline::UpdateGizmos()
    {
        if (currentlySelectedObject.expired())
        {
            SetSelectedObject(std::weak_ptr<Object>());
            return;
        }

        if (gizmo)
        {
            Object* object = currentlySelectedObject.lock().get();
            if (auto* component = dynamic_cast<SceneComponent*>(object))
            {
                gizmo->SetTransform(component->GetWorldTransform());
                gizmo->Update();
                Transform newTransform = Transform(glm::inverse(component->GetParent()->GetTransform().GetMatrix()) * gizmo->GetTransform().GetMatrix());
                component->SetTransform(newTransform);
            }

            if (auto* actor = dynamic_cast<Actor*>(object))
            {
                gizmo->SetTransform(actor->GetTransform());
                gizmo->Update();
                actor->SetTransform(gizmo->GetTransform());
            }
        }
    }

    std::weak_ptr<Object> WorldOutline::GetSelectedObject() const
    {
        return currentlySelectedObject;
    }

    void WorldOutline::SetSelectedObject(const std::weak_ptr<Object>& object)
    {
        tempWorld->GetRenderer()->ClearMeshOutlines();
        if (object.lock() == currentlySelectedObject.lock())
        {
            currentlySelectedObject.reset();
        }

        currentlySelectedObject = object;
        if (object.expired())
        {
            gizmo->SetVisible(false);
            return;
        }

        Object* localObject = object.lock().get();

        if (const auto component = dynamic_cast<SceneComponent*>(localObject))
        {
            gizmo->SetVisible(true);
            component->Select();
            gizmo->SetTransform(component->GetWorldTransform());
        }

        if (const auto actor = dynamic_cast<Actor*>(localObject))
        {
            gizmo->SetVisible(true);
            actor->Select();
            gizmo->SetTransform(actor->GetTransform());
        }
    }

    void WorldOutline::DrawObject(const std::shared_ptr<Object>& object)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (object == currentlySelectedObject.lock())
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
            if (const Actor* actor = dynamic_cast<Actor*>(object.get()))
            {
                for (const auto& sceneComponent : actor->GetAttachedComponents())
                {
                    if (ImGui::Selectable(fmt::format("\t{}", sceneComponent->GetDisplayName()).c_str(), sceneComponent == currentlySelectedObject.lock()))
                    {
                        SetSelectedObject(sceneComponent);
                    }
                }
                
                for (const std::shared_ptr<Component>& component : actor->GetComponents())
                {
                    if (ImGui::Selectable(
                        fmt::format("\t{}", component->GetDisplayName()).c_str(),
                        component == currentlySelectedObject.lock()))
                    {
                        SetSelectedObject(component);
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}
