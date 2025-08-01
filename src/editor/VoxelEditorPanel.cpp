//
// Created by steph on 6/16/2025.
//

#include "VoxelEditorPanel.h"

#include "Editor.h"
#include "core/objects/world/World.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"

namespace Vox
{
    VoxelEditorPanel::VoxelEditorPanel(const std::shared_ptr<World>& world)
        :world(world), voxelMaterialId(1)
    {
        raycastClickDelegate = ServiceLocator::GetInputService()->RegisterMouseClickCallback([this](const int x, const int y)
        {
            if (std::shared_ptr<World> worldLock = this->world.lock())
            {
                if (const std::optional<VoxelRaycastResult> result = worldLock->GetVoxels()->CastScreenSpaceRay({x, y}); result.has_value())
                {
                    const glm::ivec3 clickedVoxel = voxelMaterialId ? result.value().voxel + result.value().voxelNormal : result.value().voxel;
                    if (clickedVoxel.y <= 0 || clickedVoxel.y >= 32)
                    {
                        return;
                    }

                    Voxel newVoxel;
                    newVoxel.materialId = voxelMaterialId;
                    worldLock->GetVoxels()->SetVoxel(clickedVoxel, newVoxel);
                    worldLock->GetVoxels()->FinalizeUpdate();
                }
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