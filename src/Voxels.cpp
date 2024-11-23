#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"

#include "editor/Editor.h"
#include "rendering/Renderer.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        SetTargetFPS(60);
        rlImGuiSetup(true);

        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });

        while (!WindowShouldClose())
        {
            renderer->Render(editor.get());
        }
    }
    CloseWindow();

    return 0;
}
