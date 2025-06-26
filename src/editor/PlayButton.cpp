//
// Created by steph on 5/25/2025.
//

#include "PlayButton.h"

#include "core/objects/world/World.h"
#include "imgui.h"

namespace Vox
{
    void PlayButton::Draw(WorldState& worldState)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        constexpr auto buttonSize = ImVec2(32, 32);
        if (worldState == Inactive)
        {
            if (ImGui::Button("Play", buttonSize))
            {
                worldState = WorldState::Playing;
            }
        }
        else
        {
            if (ImGui::Button("Stop", buttonSize))
            {
                worldState = WorldState::Inactive;
            }
        }
        ImGui::SameLine();
        if (worldState == WorldState::Paused)
        {
            if (ImGui::Button("Resume", buttonSize))
            {
                worldState = WorldState::Playing;
            }
        }
        else
        {
            if (ImGui::Button("Pause", buttonSize))
            {
                worldState = WorldState::Paused;
            }
        }

        ImGui:ImGui::PopStyleVar();
    }
} // Vox