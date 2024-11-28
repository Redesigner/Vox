#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"

#include <thread>

#include "editor/Editor.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        SetTargetFPS(60);
        rlImGuiSetup(true);

        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();
        std::shared_ptr<PhysicsServer> physicsServer = std::make_unique<PhysicsServer>();

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });

        std::thread physicsThread = std::thread([physicsServer]
            {
                physicsServer->Step();
            });

        while (!WindowShouldClose())
        {
            renderer->Render(editor.get());
        }

        physicsThread.join();
    }

    CloseWindow();

    return 0;
}
