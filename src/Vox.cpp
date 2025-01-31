#include "Vox.h"

#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_video.h>

#include "core/logging/Logging.h"
#include "core/math/Math.h"
#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "physics/CharacterController.h"
#include "physics/PhysicsServer.h"
#include "physics/SpringArm.h"
#include "physics/TypeConversions.h"
#include "rendering/Camera.h"
#include "rendering/DebugRenderer.h"
#include "rendering/Renderer.h"
#include "voxel/CollisionOctree.h"
#include "voxel/VoxelChunk.h"

int main()
{
    using namespace Vox;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        VoxLog(Error, Rendering, "Failed to init SDL: {}", SDL_GetError());
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetSwapInterval(1);

    SDL_Window* window = SDL_CreateWindow("Vox", 800, 450, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    GLenum glewResult = glewInit();

    // Initialize OpenGL bindings
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        {
            std::string typeString = Vox::Renderer::GetGlDebugTypeString(type);

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                // VoxLog(Display, Rendering, "OpenGL Notification {}: {}", typeString, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                VoxLog(Warning, Rendering, "OpenGL Callback {}: {}", typeString, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                VoxLog(Warning, Rendering, "OpenGL Callback {}: {}", typeString, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                VoxLog(Error, Rendering, "OpenGL Error {}: {}", typeString, message);
                break;
            }
        }, 0); 

    // Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(800, 450);
    //io.Fonts->AddFontFromFileTTF("includes/fonts/arial.ttf", 14.0f);
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
    {
        ServiceLocator::InitServices(window);

        ServiceLocator::GetInputService()->RegisterKeyboardCallback(SDL_SCANCODE_F11, [](bool pressed) {/* if (pressed) ToggleBorderlessWindowed();*/ });

        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(window);
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();
        std::shared_ptr<PhysicsServer> physicsServer = std::make_unique<PhysicsServer>();
        std::shared_ptr<DebugRenderer> debugRenderer = std::make_shared<DebugRenderer>();
        Renderer* localRenderer = renderer.get(); // This is just for a test

        VoxelChunk voxelChunk = VoxelChunk(glm::ivec2(0, 0), physicsServer.get(), localRenderer);

        physicsServer->SetDebugRenderer(debugRenderer);

        Ref<CharacterController> characterController = physicsServer->CreateCharacterController(0.5f, 1.0f);
        Ref<SpringArm> springArm = physicsServer->CreateSpringArm(characterController);

        VoxLog(Display, Game, "Test log!");

        renderer->SetDebugPhysicsServer(physicsServer);
        renderer->UploadModel("mushroom", "../../../assets/models/mushroom.glb");
        Ref<MeshInstance> meshInstance = renderer->CreateMeshInstance("mushroom");
        if (meshInstance)
        {
            meshInstance->SetTransform(glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.0f, 10.0f, 0.0f)));
        }
        renderer->CreateMeshInstance("mushroom");

        Voxel defaultVoxel = Voxel();
        defaultVoxel.materialId = 1;
        for (int x = -16; x < 16; ++x)
        {
            for (int y = -16; y < 0; ++y)
            {
                for (int z = -16; z < 16; ++z)
                {
                    voxelChunk.SetVoxel(glm::uvec3(x + 16, y + 16, z + 16), defaultVoxel);
                }
            }
        }
        voxelChunk.FinalizeUpdate();

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                // renderer->LoadTestModel(fileName);
            });

        using frame60 = std::chrono::duration<double, std::ratio<1, 60>>;
        std::chrono::duration frameTime = frame60(1);
        std::atomic<bool> runPhysics = true;
        std::thread physicsThread = std::thread([physicsServer, &runPhysics, frameTime, localRenderer, springArm, &characterController]
            {
                while (runPhysics)
                {
                    std::chrono::time_point threadStartTime = std::chrono::steady_clock::now();
                    // this is not a good way to set this, but it's fine for a test, I think
                    physicsServer->Step();
                    glm::vec3 cameraPositon = Vector3From(springArm->GetResultPosition());
                    glm::vec3 cameraRotation = Vector3From(springArm->GetEulerRotation());
                    cameraRotation *= -1.0f;
                    localRenderer->SetCameraPosition(cameraPositon);
                    localRenderer->SetCameraTarget(Vector3From(springArm->GetOrigin()));
                    // localRenderer->SetCameraRotation(cameraRotation);

                    characterController->SetYaw(cameraRotation.y);

                    if (threadStartTime + frameTime < std::chrono::steady_clock::now())
                    {
                        VoxLog(Warning, Physics, "Physics thread lagging...");
                        VoxLog(Display, Physics, "Physics thread took {} ms.", std::chrono::duration_cast<std::chrono::milliseconds > (std::chrono::steady_clock::now() - threadStartTime).count());
                    }
                    std::this_thread::sleep_until(threadStartTime + frameTime);
                }
            });

        InputService* inputService = ServiceLocator::GetInputService();
        Editor* localEditor = editor.get();
        Camera* camera = renderer->GetCurrentCamera();
        inputService->RegisterMouseClickCallback([localEditor, debugRenderer, physicsServer, camera, &voxelChunk](int x, int y) {
            float xViewport, yViewport;
            if (localEditor->GetClickViewportSpace(xViewport, yViewport, x, y))
            {
                glm::vec4 rayStartViewport = glm::vec4(xViewport, yViewport, -1.0f, 1.0f);
                glm::vec4 rayEndViewport = glm::vec4(xViewport, yViewport, 1.0f, 1.0f);
                glm::mat4x4 inverseCamera = glm::inverseTranspose(camera->GetViewProjectionMatrix());
                glm::vec4 rayStartAffine = rayStartViewport * inverseCamera;
                glm::vec4 rayEndAffine = rayEndViewport * inverseCamera;
                rayStartAffine /= rayStartAffine.w;
                rayEndAffine /= rayEndAffine.w;
                JPH::Vec3 rayStart = Vec3From(rayStartAffine);
                JPH::Vec3 rayEnd = Vec3From(rayEndAffine);

                debugRenderer->DrawPersistentLine(rayStart, rayEnd, JPH::Color::sRed, 5.0f);
                VoxLog(Display, Game, "Casting ray from '({}, {}, {})' to '({}, {}, {})'", rayStart.GetX(), rayStart.GetY(), rayStart.GetZ(), rayEnd.GetX(), rayEnd.GetY(), rayEnd.GetZ());
                RayCastResultNormal raycastResult;
                if (physicsServer->RayCast(rayStart, rayEnd - rayStart, raycastResult))
                {
                    debugRenderer->DrawPersistentLine(raycastResult.impactPoint, raycastResult.impactPoint + raycastResult.impactNormal, JPH::Color::sBlue, 5.0f);
                    const unsigned int gridSize = 1;
                    const JPH::Vec3 voxelEstimate = raycastResult.impactPoint - raycastResult.impactNormal / static_cast<float>(2 * gridSize);
                    JPH::Vec3 voxelPosition = JPH::Vec3(
                        FloorMultiple(voxelEstimate.GetX(), gridSize),
                        FloorMultiple(voxelEstimate.GetY(), gridSize),
                        FloorMultiple(voxelEstimate.GetZ(), gridSize)
                    );
                    //debugRenderer->DrawPersistentLine(voxelPosition + JPH::Vec3(0.0f, gridSize + 0.1f, 0.0f), voxelPosition + JPH::Vec3(gridSize, gridSize + 0.1f, gridSize), JPH::Color::sRed, 5.0f);
                    //debugRenderer->DrawPersistentLine(voxelPosition + JPH::Vec3(0.0f, gridSize + 0.1f, gridSize), voxelPosition + JPH::Vec3(gridSize, gridSize + 0.1f, 0.0f), JPH::Color::sRed, 5.0f);
                    // TraceLog(LOG_INFO, TextFormat("Estimating voxel at (%f, %f, %f)", voxelEstimate.GetX(), voxelEstimate.GetY(), voxelEstimate.GetZ()));  
                    VoxLog(Display, Game, "Clicked voxel at ({}, {}, {})", voxelPosition.GetX(), voxelPosition.GetY(), voxelPosition.GetZ());
                    voxelPosition += raycastResult.impactNormal;
                    glm::uvec3 clickedVoxel = glm::uvec3(voxelPosition.GetX() + 16, voxelPosition.GetY() + 16, voxelPosition.GetZ() + 16);
                    if (clickedVoxel.x >= 0 && clickedVoxel.x < 32 && clickedVoxel.y >= 0 && clickedVoxel.y < 32 && clickedVoxel.z >= 0 && clickedVoxel.z < 32)
                    {
                        Voxel newVoxel = Voxel();
                        newVoxel.materialId = 1;
                        voxelChunk.SetVoxel(clickedVoxel, newVoxel);
                        voxelChunk.FinalizeUpdate();
                    }
                }
            }
            });

        while (!inputService->ShouldCloseWindow())
        {
            inputService->PollEvents();
            renderer->Render(editor.get());
        }
        runPhysics = false;
        physicsThread.join();
    }

    ServiceLocator::DeleteServices();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
