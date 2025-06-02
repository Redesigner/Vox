#include "Renderer.h"

#include <GL/glew.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_video.h>
#include <ranges>
#include <utility>

#include "PickContainer.h"
#include "SceneRenderer.h"
#include "core/logging/Logging.h"
#include "core/services/FileIOService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "editor/EditorViewport.h"
#include "physics/PhysicsServer.h"
#include "rendering/FullscreenQuad.h"
#include "rendering/buffers/ArrayTexture.h"
#include "rendering/buffers/RenderTexture.h"
#include "rendering/shaders/pixel_shaders/DeferredShader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/VoxelShader.h"
#include "shaders/pixel_shaders/DebugShader.h"
#include "shaders/pixel_shaders/SkyShader.h"
#include "shaders/pixel_shaders/mesh_shaders/PickShader.h"
#include "shaders/pixel_shaders/mesh_shaders/StencilShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderDistance.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderJump.h"

namespace Vox
{
    Renderer::Renderer(SDL_Window* window)
    {
        glEnable(GL_CULL_FACE);
        mainWindow = window;
        // This is relative to the build location -- I'll have to fix this later
        // ChangeDirectory("../../../");

        VoxLog(Display, Rendering, "Set current working directory to {}", SDL_GetBasePath());

        quad = std::make_unique<FullscreenQuad>();

        CreateMeshVao();
        CreateSkeletalMeshVao();
        CreateVoxelVao();

        UploadModel("gizmoArrow", "gizmoArrow.glb");
        LoadDefaultShaders();

        voxelTextures = std::make_unique<ArrayTexture>(64, 64, 5, 1);
        voxelTextures->LoadTexture("assets/textures/voxel0.png", 0);
        voxelTextures->LoadTexture("assets/textures/voxel1.png", 1);}

    Renderer::~Renderer()
    {
        glDeleteBuffers(1, &voxelMeshVao);
    }

    SDL_Window* Renderer::GetWindow() const
    {
        return mainWindow;
    }

#ifdef EDITOR
    void Renderer::Render(Editor* editor)
    {
        std::erase_if(activeScenes, [](const std::weak_ptr<SceneRenderer>& renderer)
        {
            return renderer.expired();
        });

        for (const std::weak_ptr<SceneRenderer>& renderer : activeScenes)
        {
            renderer.lock()->Draw();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int x, y;
        SDL_GetWindowSizeInPixels(mainWindow, &x, &y);
        glViewport(0, 0, x, y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        editor->Draw();
        SDL_GL_SwapWindow(mainWindow);
    }

    void Renderer::RegisterScene(const std::shared_ptr<SceneRenderer>& scene)
    {
        activeScenes.emplace_back(scene);
    }
#endif

    bool Renderer::UploadModel(std::string alias, const std::string& relativeFilePath)
    {
        if (uploadedMeshes.contains(alias))
        {
            VoxLog(Warning, Rendering, "Failed to upload model '{}'. A model already exists with that name.", alias);
            return false;
        }

        uploadedMeshes.emplace(alias, std::make_shared<Model>(ServiceLocator::GetFileIoService()->GetAssetPath() + "models/" + relativeFilePath));
        return true;
    }

    bool Renderer::UploadSkeletalModel(std::string alias, const std::string& relativeFilePath)
    {
        if (uploadedSkeletalMeshes.contains(alias))
        {
            VoxLog(Warning, Rendering, "Failed to upload skeletal model '{}'. A model already exists with that name.", alias);
            return false;
        }

        uploadedSkeletalMeshes.emplace(alias, std::make_shared<SkeletalModel>(ServiceLocator::GetFileIoService()->GetAssetPath() + "models/" + relativeFilePath));
        return true;
    }

    std::string Renderer::GetGlDebugTypeString(const unsigned int errorCode)
    {
        switch (errorCode)
        {
        case GL_DEBUG_TYPE_ERROR:
            return "GL_DEBUG_TYPE_ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR ";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "GL_DEBUG_TYPE_PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "GL_DEBUG_TYPE_PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:
            return "GL_DEBUG_TYPE_MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "GL_DEBUG_TYPE_PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "GL_DEBUG_TYPE_POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:
            return "GL_DEBUG_TYPE_OTHER";
        default:
            return "UNKNOWN";
        }
    }

    const std::unordered_map<std::string, std::shared_ptr<Model>>& Renderer::GetMeshes() const
    {
        return uploadedMeshes;
    }

    const std::unordered_map<std::string, std::shared_ptr<SkeletalModel>>& Renderer::GetSkeletalMeshes() const
    {
        return uploadedSkeletalMeshes;
    }

    void Renderer::LoadDefaultShaders()
    {
        voxelShader = std::make_unique<VoxelShader>();
        deferredShader = std::make_unique<DeferredShader>();
        gBufferShader = std::make_unique<MaterialShader>("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
        gBufferShaderSkeleton = std::make_unique<MaterialShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/gBuffer.frag");

        const unsigned int matrixBufferLocation = glGetUniformBlockIndex(gBufferShaderSkeleton->GetId(), "data");
        glUniformBlockBinding(gBufferShaderSkeleton->GetId(), matrixBufferLocation, 0);

        skyShader = std::make_unique<SkyShader>();
        debugTriangleShader = std::make_unique<DebugShader>("assets/shaders/debugTriangle.vert", "assets/shaders/debugTriangle.frag");
        debugLineShader = std::make_unique<DebugShader>("assets/shaders/debugLine.vert", "assets/shaders/debugLine.frag");

        voxelGenerationShader.Load("assets/shaders/voxelGeneration.comp");

#ifdef EDITOR
        overlayShader = std::make_unique<MaterialShader>("assets/shaders/forwardMaterial.vert", "assets/shaders/forwardMaterial.frag");

        pickShader = std::make_unique<PickShader>("assets/shaders/pickBuffer.vert", "assets/shaders/pickBuffer.frag");
        pickShaderSkeleton = std::make_unique<PickShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/pickBuffer.frag");

        stencilShader = std::make_unique<StencilShader>();
        stencilShaderSkeleton = std::make_unique<StencilShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/stencil.frag");


        outlineShader = std::make_unique<OutlineShader>();
        outlineShaderJump = std::make_unique<OutlineShaderJump>();
        outlineShaderDistance = std::make_unique<OutlineShaderDistance>();
#endif
    }

    void Renderer::CopyViewportToTexture(const RenderTexture& texture)
    {
        glBlitNamedFramebuffer(0, texture.GetFramebufferId(),
            0, 0, texture.GetWidth(), texture.GetHeight(),
            0, 0, texture.GetWidth(), texture.GetHeight(),
            GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, 0);
    }

    void Renderer::CreateMeshVao()
    {
        // For this format, it's easiest to assume that our different attributes are not interleaved
        // Blender exports this way
        glGenVertexArrays(1, &meshVao);
        glBindVertexArray(meshVao);
        // Position
        glEnableVertexArrayAttrib(meshVao, 0);
        glVertexArrayAttribFormat(meshVao, 0, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(meshVao, 0, 0);
        // Normal
        glEnableVertexArrayAttrib(meshVao, 1);
        glVertexArrayAttribFormat(meshVao, 1, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(meshVao, 1, 1);
        // TexCoord
        glEnableVertexArrayAttrib(meshVao, 2);
        glVertexArrayAttribFormat(meshVao, 2, 2, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(meshVao, 2, 2);
    }

    void Renderer::CreateSkeletalMeshVao()
    {
        glGenVertexArrays(1, &skeletalMeshVao);
        glBindVertexArray(skeletalMeshVao);
        // Position
        glEnableVertexArrayAttrib(skeletalMeshVao, 0);
        glVertexArrayAttribFormat(skeletalMeshVao, 0, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(skeletalMeshVao, 0, 0);
        // Normal
        glEnableVertexArrayAttrib(skeletalMeshVao, 1);
        glVertexArrayAttribFormat(skeletalMeshVao, 1, 3, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(skeletalMeshVao, 1, 1);
        // TexCoord
        glEnableVertexArrayAttrib(skeletalMeshVao, 2);
        glVertexArrayAttribFormat(skeletalMeshVao, 2, 2, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(skeletalMeshVao, 2, 2);
        // Joints
        glEnableVertexArrayAttrib(skeletalMeshVao, 3);
        glVertexArrayAttribIFormat(skeletalMeshVao, 3, 4, GL_UNSIGNED_BYTE, 0);
        glVertexArrayAttribBinding(skeletalMeshVao, 3, 3);
        // Weights
        glEnableVertexArrayAttrib(skeletalMeshVao, 4);
        glVertexArrayAttribFormat(skeletalMeshVao, 4, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(skeletalMeshVao, 4, 4);
    }

    void Renderer::CreateVoxelVao()
    {
        glGenVertexArrays(1, &voxelMeshVao);
        glBindVertexArray(voxelMeshVao);
        constexpr size_t texCoordOffset = sizeof(float) * 4;
        constexpr size_t normalOffset = sizeof(float) * 8;
        constexpr size_t materialIdOffset = sizeof(float) * 11;
        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribFormat(1, 2, GL_FLOAT, false, texCoordOffset);
        glVertexAttribFormat(2, 3, GL_FLOAT, false, normalOffset);
        glVertexAttribIFormat(3, 1, GL_UNSIGNED_INT, materialIdOffset);
        glVertexAttribBinding(0, 0);
        glVertexAttribBinding(1, 0);
        glVertexAttribBinding(2, 0);
        glVertexAttribBinding(3, 0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
    }

    std::shared_ptr<Model> Renderer::GetMesh(const std::string& name) const
    {
        const auto& result = uploadedMeshes.find(name);
        if (result == uploadedMeshes.end())
        {
            return {};
        }

        return result->second;
    }

    std::shared_ptr<SkeletalModel> Renderer::GetSkeletalMesh(const std::string& name) const
    {
        const auto& result = uploadedSkeletalMeshes.find(name);
        if (result == uploadedSkeletalMeshes.end())
        {
            return {};
        }

        return result->second;
    }

    MaterialShader* Renderer::GetGBufferShader() const
    {
        return gBufferShader.get();
    }

    void Renderer::BindMeshVao() const
    {
        glBindVertexArray(meshVao);
    }

    MaterialShader* Renderer::GetGBufferShaderSkeleton() const
    {
        return gBufferShaderSkeleton.get();
    }

    void Renderer::BindSkeletalMeshVao() const
    {
        glBindVertexArray(skeletalMeshVao);
    }

    DeferredShader* Renderer::GetDeferredShader() const
    {
        return deferredShader.get();
    }

    void Renderer::BindQuadVao() const
    {
        glBindVertexArray(quad->GetVaoId());
    }

    SkyShader* Renderer::GetSkyShader() const
    {
        return skyShader.get();
    }

    VoxelShader* Renderer::GetVoxelMeshShader() const
    {
        return voxelShader.get();
    }

    const ComputeShader* Renderer::GetVoxelGenerationShader() const
    {
        return &voxelGenerationShader;
    }

    ArrayTexture* Renderer::GetVoxelTextures() const
    {
        return voxelTextures.get();
    }

    void Renderer::BindVoxelMeshVao() const
    {
        glBindVertexArray(voxelMeshVao);
    }

    DebugShader* Renderer::GetDebugLineShader() const
    {
        return debugLineShader.get();
    }

    DebugShader* Renderer::GetDebugTriangleShader() const
    {
        return debugTriangleShader.get();
    }

    PickShader* Renderer::GetPickShader() const
    {
        return pickShader.get();
    }

    PickShader* Renderer::GetPickShaderSkeleton() const
    {
        return pickShaderSkeleton.get();
    }

    StencilShader* Renderer::GetStencilShader() const
    {
        return stencilShader.get();
    }

    StencilShader* Renderer::GetStencilShaderSkeleton() const
    {
        return stencilShaderSkeleton.get();
    }

    OutlineShader* Renderer::GetOutlineShader() const
    {
        return outlineShader.get();
    }

    OutlineShaderDistance* Renderer::GetOutlineShaderDistance() const
    {
        return outlineShaderDistance.get();
    }

    OutlineShaderJump* Renderer::GetOutlineShaderJump() const
    {
        return outlineShaderJump.get();
    }

    MaterialShader* Renderer::GetOverlayShader() const
    {
        return overlayShader.get();
    }
}
