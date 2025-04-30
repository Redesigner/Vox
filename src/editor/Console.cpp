#include "Console.h"

#include <imgui.h>

#include "Editor.h"
#include "core/logging/Logging.h"

namespace Vox
{
    Console::Console()
    {
        filter.SetLevelFilter(Verbose, false);
    }

    void Console::Draw()
    {
        const float consoleHeight = ImGui::GetTextLineHeightWithSpacing() * 8;
        const float tabHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;
        constexpr float toolbarSize = 5.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - consoleHeight - tabHeight - toolbarSize);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        if (ImGui::BeginChild("Console", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - ImGui::GetCursorPosY()), ImGuiChildFlags_Borders))
        {
            DrawTabs();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
            if (ImGui::BeginChild("ConsoleEntries", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - ImGui::GetCursorPosY()), ImGuiChildFlags_Borders))
            {
                std::vector<size_t> validEntries = Logger::GetEntriesFilteredByIndex(filter);
                const std::vector<LogEntry>& entries = Logger::GetEntries();
                for (size_t entryIndex : validEntries)
                {
                    const LogEntry& entry = entries[entryIndex];
                    glm::vec3 entryColor = Vox::Logger::GetLevelColor(entry.level);
                    ImGui::TextColored(ImVec4(entryColor.r / 255.0f, entryColor.g / 255.0f, entryColor.b / 255.0f, 1.0f), entry.entry.c_str());
                }
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleVar(1);
        ImGui::EndChild();
    }

    void Console::DrawTabs()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColorUnselected);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 0.0f));
        ImGui::PushFont(Editor::GetFont_GitLab18());
        std::vector<LogCategory>& categories = Logger::GetCategories();
        for (int i = 0; i < categories.size(); ++i)
        {
            if (filter.IsCategoryDisplayed(categories[i]))
            {
                ImGui::PushStyleColor(ImGuiCol_Button, buttonColorSelected);
                if (ImGui::Button(Logger::GetCategoryTag(categories[i]).c_str()))
                {
                    filter.SetCategoryFilter(categories[i], false);
                }
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Button(Logger::GetCategoryTag(categories[i]).c_str()))
                {
                    filter.SetCategoryFilter(categories[i], true);
                }
            }
            
            if (i < categories.size() - 1)
            {
                ImGui::SameLine();
            }
            else
            {
                ImGui::SameLine(0, 100.0f);
            }
        }
        
        std::vector<LogLevel>& levels = Logger::GetLevels();
        for (int i = 0; i < levels.size(); ++i)
        {
            if (filter.IsLevelDisplayed(levels[i]))
            {
                ImGui::PushStyleColor(ImGuiCol_Button, buttonColorSelected);
                if (ImGui::Button(Logger::GetLevelTag(levels[i]).c_str()))
                {
                    filter.SetLevelFilter(levels[i], false);
                }
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Button(Logger::GetLevelTag(levels[i]).c_str()))
                {
                    filter.SetLevelFilter(levels[i], true);
                }
            }
            
            if (i < levels.size() - 1)
            {
                ImGui::SameLine();
            }
        }
        ImGui::PopFont();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor();
    }

    ImVec4 Console::buttonColorUnselected = ImVec4(0.125f, 0.121f, 0.137f, 1.0f);
    ImVec4 Console::buttonColorSelected = ImVec4(51.0f / 255.0f, 80.0f / 255.0f, 155.0f / 255.0f, 1.0f);
}
