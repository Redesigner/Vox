#include "WorldOutline.h"

#include <imgui.h>

#include "Editor.h"
#include "core/objects/Object.h"
#include "core/objects/world/World.h"
#include "../game_objects/actors/Actor.h"
#include "../game_objects/components/Component.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"
#include "rendering/gizmos/Gizmo.h"

namespace Vox
{
    WorldOutline::WorldOutline()
    {
        tempWorld = nullptr;
        gizmo = nullptr;
    }

    WorldOutline::~WorldOutline() = default;

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
                for (const auto& object : world->GetActors())
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
            gizmo->SetVisible(false);
            return;
        }

        if (gizmo)
        {
            Object* object = currentlySelectedObject.lock().get();
            if (auto* component = dynamic_cast<SceneComponent*>(object))
            {
                gizmo->SetTransform(component->GetWorldTransform());
                gizmo->Update();

                Transform newTransform;
                if (const SceneComponent* parentAttachment = component->GetParentAttachment())
                {
                    newTransform = Transform(glm::inverse(parentAttachment->GetWorldTransform().GetMatrix()) * gizmo->GetTransform().GetMatrix());
                }
                else
                {
                    newTransform = gizmo->GetTransform();
                }
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

    void WorldOutline::SetDoubleClickCallback(ClickCallback callback)
    {
        doubleClickCallback = std::move(callback);
    }

    void WorldOutline::DrawObject(const std::shared_ptr<Object>& object)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (object == currentlySelectedObject.lock())
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const auto& objectClass = object->GetClass();
        const std::string& className = objectClass->GetName();
        const std::string& objectName = object->GetDisplayName();
        const bool itemExpanded = ImGui::TreeNodeEx(fmt::format("{} {}", className, objectName).c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            SetSelectedObject(object);
        }
        if (itemExpanded)
        {
            if (const Actor* actor = dynamic_cast<Actor*>(object.get()))
            {
                for (const std::shared_ptr<Component>& component : actor->GetChildren())
                {
                    DrawComponent(component);
                }
                DrawSceneComponent(actor->GetRootComponent());
            }
            ImGui::TreePop();
        }
    }

    void WorldOutline::DrawComponent(const std::shared_ptr<Component>& component)
    {
        if (!component->GetClass()->IsA<SceneComponent>())
        {
            if (ImGui::Selectable(
            fmt::format("\t{}", component->GetDisplayName()).c_str(),
            component == currentlySelectedObject.lock()))
                {
                    SetSelectedObject(component);
                }
        }
    }

    void WorldOutline::DrawSceneComponent(const std::shared_ptr<SceneComponent>& sceneComponent) // NOLINT(*-no-recursion)
    {
        if (sceneComponent->GetAttachments().empty())
        {
            if (ImGui::Selectable(fmt::format("\t{}{}", sceneComponent->native ? "(N) " : "", sceneComponent->GetDisplayName()).c_str()))
            {
                SetSelectedObject(std::static_pointer_cast<Object>(sceneComponent));
            }
        }
        else
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            if (sceneComponent == currentlySelectedObject.lock())
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            const bool itemExpanded = ImGui::TreeNodeEx(
                fmt::format("{}{} {}",
                    sceneComponent->native ? "(N) " : "",
                    sceneComponent->GetClassDisplayName(),
                    sceneComponent->GetDisplayName()).c_str(),
                flags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                SetSelectedObject(std::static_pointer_cast<Object>(sceneComponent));
            }
            if (itemExpanded)
            {
                for (const auto& attachments : sceneComponent->GetAttachments())
                {
                    DrawSceneComponent(attachments);
                }
                ImGui::TreePop();
            }
        }
    }
}
