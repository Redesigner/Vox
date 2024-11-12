#include "Voxels.h"

#include "raylib.h"

#include "imgui.h"
#include "rlImGui.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "raylib [core] example - basic window");
    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        // start ImGui Conent
        rlImGuiBegin();

        // show ImGui Content
        bool open = true;
        ImGui::ShowDemoWindow(&open);
        rlImGuiEnd();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
