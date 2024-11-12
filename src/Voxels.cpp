#include "Voxels.h"

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "editor/Editor.h"

int main()
{
    InitWindow(800, 450, "raylib [core] example - basic window");
    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        // start ImGui Conent
        rlImGuiBegin();

#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
#else 
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

        Editor::Draw();

        rlImGuiEnd();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
