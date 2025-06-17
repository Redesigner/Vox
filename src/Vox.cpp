#include "Vox.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec3.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "game_objects/actors/character/Character.h"
#include "core/config/Config.h"
#include "core/logging/Logging.h"
#include "core/math/Math.h"
#include "core/math/Formatting.h"
#include "core/objects/prefabs/Prefab.h"
#include "core/objects/World.h"
#include "core/services/EditorService.h"
#include "core/services/FileIOService.h"
#include "core/services/InputService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "editor/EditorViewport.h"
#include "physics/PhysicsServer.h"
#include "physics/TypeConversions.h"
#include "rendering/camera/Camera.h"
#include "rendering/DebugRenderer.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"
#include "voxel/VoxelChunk.h"
#include "voxel/VoxelWorld.h"

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

    VoxConfig config;
    config.Load();

    SDL_Window* window = SDL_CreateWindow("Vox", config.windowSize.x, config.windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition(window, config.windowPosition.x, config.windowPosition.y);
    const SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    // SDL_GL_SetSwapInterval(1);
    if (config.windowMaximized)
    {
        SDL_MaximizeWindow(window);
    }

    /*GLenum glewResult = */ glewInit();

    // Initialize OpenGL debug bindings
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, const GLenum type, GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        {
            std::string typeString = Vox::Renderer::GetGlDebugTypeString(type);

            // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
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
        }, nullptr);

    // Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(800, 450);
    //io.Fonts->AddFontFromFileTTF("includes/fonts/arial.ttf", 14.0f);
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    // INITIALIZE RESOURCES FOR MAIN LOOP
    {
        ServiceLocator::InitServices(window);

        auto testWorld = std::make_shared<World>();
        testWorld->InitializeVoxels();
        auto debugRenderer = std::make_shared<DebugRenderer>();

#ifdef EDITOR
        ServiceLocator::GetEditorService()->GetEditor()->SetWorld(testWorld);
        ServiceLocator::GetRenderer()->RegisterScene(testWorld->GetRenderer());
#endif

        testWorld->GetPhysicsServer()->SetDebugRenderer(debugRenderer);

        ServiceLocator::GetRenderer()->UploadModel("witch", "witch.glb");
        ServiceLocator::GetRenderer()->UploadSkeletalModel("scorpion", "scorpion.glb");
        //ServiceLocator::GetRenderer()->UploadSkeletalModel("cube", "../../../assets/models/animatedCube.glb");

        ServiceLocator::GetEditorService()->GetEditor()->InitializeGizmos(testWorld.get());

        using frame60 = std::chrono::duration<double, std::ratio<1, 60>>;
        auto frameTime = frame60(1);
        std::atomic runPhysics = true;
        auto physicsThread = std::thread([&runPhysics, frameTime, testWorld]
            {
                while (runPhysics)
                {
                    std::chrono::time_point threadStartTime = std::chrono::steady_clock::now();
                    testWorld->GetPhysicsServer()->Step();

                    if (threadStartTime + frameTime < std::chrono::steady_clock::now())
                    {
                        VoxLog(Warning, Physics, "Physics thread lagging...");
                        VoxLog(Display, Physics, "Physics thread took {} ms.", std::chrono::duration_cast<std::chrono::milliseconds > (std::chrono::steady_clock::now() - threadStartTime).count());
                    }
                    std::this_thread::sleep_until(threadStartTime + frameTime);
                }
            });

        ServiceLocator::GetObjectService()->RegisterPrefab("test.json");

        ServiceLocator::GetEditorService()->GetEditor()->SetWorld(testWorld);
        testWorld->Load(SavedWorld(ServiceLocator::GetFileIoService()->LoadFile("worlds/MainWorld.world")));

        while (!ServiceLocator::GetInputService()->ShouldCloseWindow())
        {
            ServiceLocator::GetInputService()->PollEvents();
            testWorld->Tick(1 / 60.0f);
            ServiceLocator::GetRenderer()->Render(ServiceLocator::GetEditorService()->GetEditor());
        }
        runPhysics = false;
        physicsThread.join();
    }
    int x, y, w, h;
    if (!ServiceLocator::GetInputService()->IsWindowFullscreen())
    {
        SDL_GetWindowPosition(window, &x, &y);
        SDL_GetWindowSizeInPixels(window, &w, &h);
        config.windowPosition = glm::ivec2(x, y);
        config.windowSize = glm::ivec2(w, h);
    }
    config.windowMaximized = ServiceLocator::GetInputService()->IsWindowMaximized();
    config.Write();

    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    ServiceLocator::DeleteServices();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
