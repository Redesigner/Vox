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

        renderer->SetDebugPhysicsServer(physicsServer);

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });


        std::atomic<bool> runPhysics = true;
        std::thread physicsThread = std::thread([physicsServer, &runPhysics]
            {
                while (runPhysics)
                {
                    physicsServer->Step();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
                }
            });

        JPH::BodyID boxId = physicsServer->CreateStaticBox(JPH::Vec3(32.0f, 16.0f, 32.0f), JPH::Vec3(0.0f, -8.0f, 0.0f));
        JPH::BodyID playerCapsuleId = physicsServer->CreatePlayerCapsule(0.2f, 0.5f, JPH::Vec3(2.0f, 5.0f, 2.0f));

        while (!WindowShouldClose())
        {
            JPH::Vec3 playerPhysicsPosition = physicsServer->GetObjectPosition(playerCapsuleId);
            Vector3 playerPosition = Vector3(playerPhysicsPosition.GetX(), playerPhysicsPosition.GetY(), playerPhysicsPosition.GetZ());
            renderer->SetCapsulePosition(playerPosition);
            renderer->Render(editor.get());
        }
        runPhysics = false;
        physicsThread.join();
    }

    CloseWindow();

    return 0;
}
