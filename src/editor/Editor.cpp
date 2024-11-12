#include "Editor.h"

#include "imgui.h"

void Editor::Draw()
{
    bool windowOpen = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.2f)));
    ImGui::Begin("Main Window", &windowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

	DrawToolbar();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void Editor::DrawToolbar()
{
    if (ImGui::BeginMenuBar())
    {
        DrawFileToolbar();
        ImGui::MenuItem("Edit");
        ImGui::MenuItem("Tools");
        DrawImportToolbar();
        ImGui::EndMenuBar();
    }
}

void Editor::DrawFileToolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New");
        ImGui::MenuItem("Open");
        ImGui::MenuItem("Save");
        ImGui::Separator();
        ImGui::MenuItem("Exit");
        ImGui::EndMenu();
    }
    ImGui::PopStyleVar();
}

void Editor::DrawImportToolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
    if (ImGui::BeginMenu("Import"))
    {
        if (ImGui::MenuItem("Model (gltf)"))
        {
        }
        ImGui::EndMenu();
    }
    ImGui::PopStyleVar();
}