﻿#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#include <thread>
#include <chrono>

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "rendering/Renderer.h"
#include "physics/PhysicsServer.h"
#include "physics/CharacterController.h"
#include "physics/TypeConversions.h"
#include "voxel/CollisionOctree.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        SetTargetFPS(60);
        rlImGuiSetup(true);

        Vox::ServiceLocator::InitServices();

        Vox::ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_F11, [](bool pressed) { if (pressed) ToggleBorderlessWindowed(); });

        std::unique_ptr<Vox::Renderer> renderer = std::make_unique<Vox::Renderer>();
        Vox::Renderer* localRenderer = renderer.get(); // This is just for a test
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();
        std::shared_ptr<Vox::PhysicsServer> physicsServer = std::make_unique<Vox::PhysicsServer>();

        unsigned int characterControllerId = physicsServer->CreateCharacterController(0.5f, 1.0f);
        unsigned int springArmId = physicsServer->CreateSpringArm(characterControllerId);

        renderer->SetDebugPhysicsServer(physicsServer);

        Octree::CollisionNode collisionTest = Octree::CollisionNode(32);
        Octree::PhysicsVoxel testVoxel = Octree::PhysicsVoxel();
        testVoxel.solid = true;
        for (int x = -16; x < 16; ++x)
        {
            for (int y = -16; y < 0; ++y)
            {
                for (int z = -16; z < 16; ++z)
                {
                    collisionTest.SetVoxel(x, y, z, &testVoxel);
                }
            }
        }
        collisionTest.SetVoxel(5, 2, 0, &testVoxel);
        collisionTest.SetVoxel(2, 1, 0, &testVoxel);

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });

        using frame60 = std::chrono::duration<double, std::ratio<1, 60>>;
        std::chrono::duration frameTime = frame60(1);
        std::atomic<bool> runPhysics = true;
        std::thread physicsThread = std::thread([physicsServer, &runPhysics, frameTime, localRenderer, springArmId, characterControllerId]
            {
                while (runPhysics)
                {
                    std::chrono::time_point threadStartTime = std::chrono::steady_clock::now();
                    // this is not a good way to set this, but it's fine for a test, I think
                    Vector3 cameraPositon = Vector3From(physicsServer->GetSpringArmResult(springArmId));
                    Vector3 cameraRotation = Vector3From(physicsServer->GetSpringArmEulerRotation(springArmId));
                    cameraRotation *= -1.0f;
                    localRenderer->SetCameraPosition(cameraPositon);
                    localRenderer->SetCameraTarget(Vector3From(physicsServer->GetSpringArmOrigin(springArmId)));
                    // localRenderer->SetCameraRotation(cameraRotation);
                    physicsServer->Step();
                    if (threadStartTime + frameTime < std::chrono::steady_clock::now())
                    {
                        TraceLog(LOG_WARNING, "Physics thread lagging...");
                        TraceLog(LOG_INFO, TextFormat("Physics thread took %i ms.", std::chrono::duration_cast<std::chrono::milliseconds > (std::chrono::steady_clock::now() - threadStartTime).count()));
                    }
                    std::this_thread::sleep_until(threadStartTime + frameTime);
                }
            });

        // JPH::BodyID boxId = physicsServer->CreateStaticBox(JPH::Vec3(32.0f, 16.0f, 32.0f), JPH::Vec3(0.0f, -8.0f, 0.0f));
        physicsServer->CreateCompoundShape(collisionTest.MakeCompoundShape());
        JPH::BodyID playerCapsuleId = physicsServer->CreatePlayerCapsule(1.0f, 0.5f, JPH::Vec3(2.0f, 5.0f, 2.0f));

        Vox::InputService* inputService = Vox::ServiceLocator::GetInputService();
        while (!inputService->ShouldCloseWindow())
        {
            inputService->PollEvents();
            JPH::Vec3 playerPhysicsPosition = physicsServer->GetObjectPosition(playerCapsuleId);
            Vector3 playerPosition = Vector3(playerPhysicsPosition.GetX(), playerPhysicsPosition.GetY(), playerPhysicsPosition.GetZ());
            renderer->Render(editor.get());
        }
        runPhysics = false;
        physicsThread.join();
    }

    Vox::ServiceLocator::DeleteServices();
    CloseWindow();

    return 0;
}
