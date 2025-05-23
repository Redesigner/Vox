//
// Created by steph on 5/22/2025.
//

#include "ActorEditor.h"

#include <imgui.h>

#include "core/objects/World.h"
#include "editor/Editor.h"
#include "editor/EditorViewport.h"
#include "editor/WorldOutline.h"
#include "editor/detail_panel/DetailPanel.h"
#include "rendering/PickContainer.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    ActorEditor::ActorEditor()
    {
        world = std::make_shared<World>();
        viewport = std::make_shared<EditorViewport>();
        viewport->OnObjectSelected = [this](const std::shared_ptr<Object>& object)
            {
                outline->SetSelectedObject(object);
            };
        outline = std::make_unique<WorldOutline>();
        outline->InitializeGizmos(world.get());

        world->GetRenderer()->viewport = viewport;

        ServiceLocator::GetRenderer()->RegisterScene(world->GetRenderer());
    }

    ActorEditor::~ActorEditor() = default;

    void ActorEditor::Draw()
    {
        ImGui::BeginChild("ActorEditorViewportFrame", ImVec2(200, 0), ImGuiChildFlags_ResizeX);
        viewport->Draw(world);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("ActorEditorRightPane", ImVec2(0, 0));
            ImGui::BeginChild("ActorEditorOutlineFrame", ImVec2(0, 100), ImGuiChildFlags_ResizeY);
            outline->Draw(world.get());
            ImGui::EndChild();

            ImGui::BeginChild("ActorEditorDetailFrame"), ImVec2(0, 0);
            const std::string tabName = fmt::format("{}###ActorDetailPanelTab", outline->GetSelectedObject().expired() ?
                "No Object Selected" :
                outline->GetSelectedObject().lock()->GetDisplayName());
            Editor::BeginEmptyTab(tabName);
            if (!outline->GetSelectedObject().expired())
            {
                DetailPanel::Draw(outline->GetSelectedObject().lock().get());
            }
            Editor::EndEmptyTab();
        ImGui::EndChild();

        ImGui::EndChild();
    }
} // Vox