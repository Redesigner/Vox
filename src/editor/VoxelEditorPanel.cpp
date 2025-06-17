//
// Created by steph on 6/16/2025.
//

#include "VoxelEditorPanel.h"

#include "Editor.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    VoxelEditorPanel::VoxelEditorPanel(VoxelWorld* world)
        :world(world), voxelMaterialId(1)
    {
        raycastClickDelegate = ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](const int x, const int y)
        {
            if (const std::optional<VoxelRaycastResult> result = this->world->CastScreenSpaceRay({x, y}); result.has_value())
            {
                const glm::ivec3 clickedVoxel = voxelMaterialId ? result.value().voxel + result.value().voxelNormal : result.value().voxel;
                if (clickedVoxel.y <= 0 || clickedVoxel.y >= 32)
                {
                    return;
                }

                Voxel newVoxel;
                newVoxel.materialId = voxelMaterialId;
                this->world->SetVoxel(clickedVoxel, newVoxel);
                this->world->FinalizeUpdate();
            }
        });
    }

    VoxelEditorPanel::~VoxelEditorPanel()
    {
        ServiceLocator::GetInputService()->UnregisterMouseClickCallback(raycastClickDelegate);
    }

    void VoxelEditorPanel::Draw()
    {
        Editor::BeginEmptyTab("Voxels");

        ImGui::PushFont(Editor::GetFont_GitLab14());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Editor::lightBgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::BeginChild("VoxelsInternalPanel"))
        {
            ImGui::SliderInt("Material", &voxelMaterialId, 0, 6);
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        Editor::EndEmptyTab();
    }
} // Vox