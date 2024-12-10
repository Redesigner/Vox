﻿#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"

#include <thread>
#include <chrono>

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"
#include "physics/CharacterController.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        Vox::ServiceLocator::InitServices();

        SetTargetFPS(60);
        rlImGuiSetup(true);

        std::unique_ptr<Vox::Renderer> renderer = std::make_unique<Vox::Renderer>();
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();
        std::shared_ptr<Vox::PhysicsServer> physicsServer = std::make_unique<Vox::PhysicsServer>();
        std::unique_ptr<Vox::CharacterController> characterController = std::make_unique<Vox::CharacterController>(physicsServer->GetPhysicsSystem());
        Vox::CharacterController* directController = characterController.get(); // Do not do this!

        renderer->SetDebugPhysicsServer(physicsServer);

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });

        using frame60 = std::chrono::duration<double, std::ratio<1, 60>>;
        std::chrono::duration frameTime = frame60(1);
        std::atomic<bool> runPhysics = true;
        std::thread physicsThread = std::thread([physicsServer, &runPhysics, frameTime, directController]
            {
                while (runPhysics)
                {
                    std::chrono::time_point threadStartTime = std::chrono::steady_clock::now();
                    directController->Update(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime).count() / 1000.0f, physicsServer.get());
                    physicsServer->Step();
                    std::this_thread::sleep_until(threadStartTime + frameTime);
                }
            });

        JPH::BodyID boxId = physicsServer->CreateStaticBox(JPH::Vec3(32.0f, 16.0f, 32.0f), JPH::Vec3(0.0f, -8.0f, 0.0f));
        JPH::BodyID playerCapsuleId = physicsServer->CreatePlayerCapsule(0.5f, 0.5f, JPH::Vec3(2.0f, 5.0f, 2.0f));

        Vox::InputService* inputService = Vox::ServiceLocator::GetInputService();
        while (!inputService->ShouldCloseWindow())
        {
            JPH::Vec3 characterPosition = characterController->GetPosition();
            TraceLog(LOG_INFO, TextFormat("Character: (%f, %f, %f)", characterPosition.GetX(), characterPosition.GetY(), characterPosition.GetZ()));
            inputService->PollEvents();
            JPH::Vec3 playerPhysicsPosition = physicsServer->GetObjectPosition(playerCapsuleId);
            Vector3 playerPosition = Vector3(playerPhysicsPosition.GetX(), playerPhysicsPosition.GetY(), playerPhysicsPosition.GetZ());
            renderer->SetCapsulePosition(playerPosition);
            renderer->Render(editor.get());
        }
        runPhysics = false;
        physicsThread.join();
    }

    Vox::ServiceLocator::DeleteServices();
    CloseWindow();

    return 0;
}
