#include "Renderer.h"

#include <GL/glew.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_video.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <ranges>
#include <utility>

#include "PickContainer.h"
#include "buffers/frame_buffers/PickBuffer.h"
#include "buffers/frame_buffers/StencilBuffer.h"
#include "buffers/frame_buffers/UVec2Buffer.h"
#include "core/logging/Logging.h"
#include "core/math/Math.h"
#include "core/services/EditorService.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "editor/EditorViewport.h"
#include "physics/PhysicsServer.h"
#include "rendering/Camera.h"
#include "rendering/DebugRenderer.h"
#include "rendering/FullscreenQuad.h"
#include "rendering/buffers/ArrayTexture.h"
#include "rendering/buffers/RenderTexture.h"
#include "rendering/buffers/frame_buffers/ColorDepthFramebuffer.h"
#include "rendering/buffers/frame_buffers/GBuffer.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/shaders/pixel_shaders/DeferredShader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/VoxelShader.h"
#include "shaders/pixel_shaders/mesh_shaders/PickShader.h"
#include "shaders/pixel_shaders/mesh_shaders/StencilShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderDistance.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderJump.h"
#include "skeletal_mesh/SkeletalMeshInstanceContainer.h"

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

        voxelShader = std::make_unique<VoxelShader>();
        deferredShader = std::make_unique<DeferredShader>();
        gBufferShader = std::make_unique<MaterialShader>("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
        gBufferShaderSkeleton = std::make_unique<MaterialShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/gBuffer.frag");

        const unsigned int matrixBufferLocation = glGetUniformBlockIndex(gBufferShaderSkeleton->GetId(), "data");
        glUniformBlockBinding(gBufferShaderSkeleton->GetId(), matrixBufferLocation, 0);

        skyShader = std::make_unique<PixelShader>("assets/shaders/sky.vert", "assets/shaders/sky.frag");
        debugTriangleShader = std::make_unique<PixelShader>("assets/shaders/debugTriangle.vert", "assets/shaders/debugTriangle.frag");
        debugLineShader = std::make_unique<PixelShader>("assets/shaders/debugLine.vert", "assets/shaders/debugLine.frag");

        debugTriangleMatrixLocation = debugTriangleShader->GetUniformLocation("viewProjection");
        debugLineMatrixLocation = debugLineShader->GetUniformLocation("viewProjection");

        voxelGenerationShader.Load("assets/shaders/voxelGeneration.comp");

        CreateMeshVao();
        CreateSkeletalMeshVao();
        CreateVoxelVao();

        constexpr int defaultWidth = 800;
        constexpr int defaultHeight = 450;

        gBuffer = std::make_unique<GBuffer>(defaultWidth, defaultHeight);
        deferredFramebuffer = std::make_unique<ColorDepthFramebuffer>(defaultWidth, defaultHeight);
        
#ifdef EDITOR
        overlayShader = std::make_unique<MaterialShader>("assets/shaders/forwardMaterial.vert", "assets/shaders/forwardMaterial.frag");

        pickBuffer = std::make_unique<PickBuffer>(defaultWidth, defaultHeight);
        pickShader = std::make_unique<PickShader>("assets/shaders/pickBuffer.vert", "assets/shaders/pickBuffer.frag");
        pickShaderSkeleton = std::make_unique<PickShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/pickBuffer.frag");
        pickContainer = std::make_unique<PickContainer>();

        stencilBuffer = std::make_unique<StencilBuffer>(defaultWidth, defaultHeight);
        stencilShader = std::make_unique<StencilShader>();
        stencilShaderSkeleton = std::make_unique<StencilShader>("assets/shaders/skeletalMesh.vert", "assets/shaders/stencil.frag");

        outlineBuffer = std::make_unique<UVec2Buffer>(defaultWidth, defaultHeight);
        outlineBuffer2 = std::make_unique<UVec2Buffer>(defaultWidth, defaultHeight);

        outlineShader = std::make_unique<OutlineShader>();
        outlineShaderJump = std::make_unique<OutlineShaderJump>();
        outlineShaderDistance = std::make_unique<OutlineShaderDistance>();
#endif


        skyShader->Enable();
        Vox::PixelShader::SetUniformInt(skyShader->GetUniformLocation("color"), 0);
        Vox::PixelShader::SetUniformInt(skyShader->GetUniformLocation("depth"), 1);

        voxelTextures = std::make_unique<ArrayTexture>(64, 64, 5, 1);
        voxelTextures->LoadTexture("assets/textures/voxel0.png", 0);
        voxelTextures->LoadTexture("assets/textures/voxel1.png", 1);

        lightUniformLocations = LightUniformLocations(deferredShader.get());
        testLight = Light(1, 1, glm::vec3(4.5f, 4.5f, 0.5f), glm::vec3(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f);
    }

    Renderer::~Renderer()
    {
        glDeleteBuffers(1, &voxelMeshVao);
    }

    void Renderer::Render(Editor* editor)
    {
        CheckViewportDimensions(editor);
        currentCamera->SetAspectRatio(viewportTexture->GetHeight() == 0 ? 1 : viewportTexture->GetWidth() / viewportTexture->GetHeight());
        glViewport(0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight());

        RenderGBuffer();
        RenderDeferred();
        RenderSky();
        // RenderDebugShapes();

#ifdef EDITOR
        RenderPickBuffer();
        RenderOutline();
        RenderOverlay();
#endif

        int width, height;
        SDL_GetWindowSizeInPixels(mainWindow, &width, &height);

        glBlitNamedFramebuffer(deferredFramebuffer->GetFramebufferId(), 0,
            0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight(),
            0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
        // Make sure that our viewport size matches the window size when drawing with imgui
        // glViewport(0, 0, width, height);
        // glViewport(0, 0, 800, 450);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        editor->Draw(deferredFramebuffer.get());
        SDL_GL_SwapWindow(mainWindow);
    }

    Ref<Camera> Renderer::CreateCamera()
    {
        Ref<Camera> newCamera = Ref<Camera>(&cameras, cameras.Create());
        newCamera->SetPosition(0.0f, 0.0f, 0.0f);
        newCamera->SetRotation(0.0f, 0.0f, 0.0f);
        // newCamera->SetAspectRatio(viewportTexture->GetAspectRatio());
        newCamera->SetFovY(45.0f);
        return newCamera;
    }

    Ref<Camera> Renderer::GetCurrentCamera() const
    {
        return currentCamera;
    }

    void Renderer::SetCurrentCamera(const Ref<Camera>& camera)
    {
        currentCamera = camera;
    }

    bool Renderer::UploadModel(std::string alias, const std::string& relativeFilePath)
    {
        if (uploadedModels.contains(alias))
        {
            VoxLog(Warning, Rendering, "Failed to upload model '{}'. A model already exists with that name.", alias);
            return false;
        }

        auto newMeshInstance = uploadedModels.emplace(alias, 8);
        newMeshInstance.first->second.LoadMesh(relativeFilePath);
        return true;
    }

    bool Renderer::UploadSkeletalModel(std::string alias, const std::string& relativeFilePath)
    {
        if (uploadedSkeletalModels.contains(alias))
        {
            VoxLog(Warning, Rendering, "Failed to upload skeletal model '{}'. A model already exists with that name.", alias);
            return false;
        }

        auto newSkeletalMeshInstance = uploadedSkeletalModels.emplace(alias, 8);
        newSkeletalMeshInstance.first->second.LoadMesh(relativeFilePath);
        return true;
    }

#ifdef EDITOR
    void Renderer::AddMeshOutline(const Ref<MeshInstance>& mesh)
    {
        if (std::ranges::find(outlinedMeshes, mesh) == outlinedMeshes.end())
        {
            outlinedMeshes.emplace_back(mesh);
        }
    }

    void Renderer::AddMeshOutline(const Ref<SkeletalMeshInstance>& mesh)
    {
        if (std::ranges::find(outlinedSkeletalMeshes, mesh) == outlinedSkeletalMeshes.end())
        {
            outlinedSkeletalMeshes.emplace_back(mesh);
        }
    }

    void Renderer::ClearMeshOutlines()
    {
        outlinedMeshes.clear();
        outlinedSkeletalMeshes.clear();
    }
#endif

    Ref<MeshInstance> Renderer::CreateMeshInstance(const std::string& meshName)
    {
        const auto mesh = uploadedModels.find(meshName);
        if (mesh == uploadedModels.end())
        {
            VoxLog(Error, Rendering, "No mesh with name '{}' exists. Make sure to call 'UploadModel' first.", meshName);
            return {};
        }

        return mesh->second.CreateMeshInstance();
    }

    Ref<SkeletalMeshInstance> Renderer::CreateSkeletalMeshInstance(const std::string& meshName)
    {
        const auto mesh = uploadedSkeletalModels.find(meshName);
        if (mesh == uploadedSkeletalModels.end())
        {
            VoxLog(Error, Rendering, "No skeletal mesh with name '{}' exists. Make sure to call 'UploadModel' first.", meshName);
            return {};
        }

        return mesh->second.CreateMeshInstance();
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

    DynamicRef<VoxelMesh> Renderer::CreateVoxelMesh(glm::ivec2 position)
    {
        return {&voxelMeshes, voxelMeshes.Create(position)};
    }

    const std::unordered_map<std::string, MeshInstanceContainer>& Renderer::GetMeshes() const
    {
        return uploadedModels;
    }

    const std::unordered_map<std::string, SkeletalMeshInstanceContainer>& Renderer::GetSkeletalMeshes() const
    {
        return uploadedSkeletalModels;
    }

#ifdef EDITOR
    PickContainer* Renderer::GetPickContainer() const
    {
        return pickContainer.get();
    }

    PickBuffer* Renderer::GetPickBuffer() const
    {
        return pickBuffer.get();
    }

    void Renderer::RegisterOverlayMesh(const Ref<MeshInstance>& meshInstance)
    {
        overlayMeshes.emplace_back(meshInstance);
    }

    void Renderer::ClearOverlays()
    {
        overlayMeshes.clear();
    }
#endif

    void Renderer::CheckViewportDimensions(const Editor* editor)
    {
        // Resize our render texture if it's the wrong size, so we get a 1:1 resolution for the editor viewport
        const glm::vec2 editorViewportSize = editor->GetViewport()->GetDimensions();
        if (viewportTexture)
        {
            const int viewportWidth = static_cast<int>(editorViewportSize.x);
            const int viewportHeight = static_cast<int>(editorViewportSize.y);
            if (viewportTexture->GetWidth() != viewportWidth || viewportTexture->GetHeight() != viewportHeight)
            {
                ResizeBuffers(viewportWidth, viewportHeight);
            }
        }
        else
        {
            viewportTexture = std::make_unique<RenderTexture>(800, 450);
        }
    }

    void Renderer::ResizeBuffers(int width, int height)
    {
        VoxLog(Display, Rendering, "Resizing GBuffer and Viewport texture to '({}, {})'", width, height);

        glViewport(0, 0, width, height);
        viewportTexture.reset();
        viewportTexture = std::make_unique<RenderTexture>(width, height);

        gBuffer.reset();
        gBuffer = std::make_unique<GBuffer>(width, height);

        deferredFramebuffer.reset();
        deferredFramebuffer = std::make_unique<ColorDepthFramebuffer>(width, height);

#ifdef EDITOR
        pickBuffer.reset();
        pickBuffer = std::make_unique<PickBuffer>(width, height);
        stencilBuffer.reset();
        stencilBuffer = std::make_unique<StencilBuffer>(width, height);

        outlineBuffer.reset();
        outlineBuffer = std::make_unique<UVec2Buffer>(width, height);
        outlineBuffer2.reset();
        outlineBuffer2 = std::make_unique<UVec2Buffer>(width, height);
#endif
    }

    void Renderer::RenderGBuffer()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        gBufferShader->Enable();
        gBufferShader->SetCamera(currentCamera);
        
        glBindVertexArray(meshVao);

        for (auto& val : uploadedModels | std::views::values)
        {
            val.Render(gBufferShader.get());
        }

        gBufferShaderSkeleton->Enable();
        gBufferShaderSkeleton->SetCamera(currentCamera);
        glBindVertexArray(skeletalMeshVao);

        for (SkeletalMeshInstanceContainer& val : uploadedSkeletalModels | std::views::values)
        {
            val.Render(gBufferShaderSkeleton.get());
        }

        UpdateVoxelMeshes();
        RenderVoxelMeshes();
    }

    void Renderer::RenderDeferred()
    {
        glBlitNamedFramebuffer(gBuffer->GetFramebufferId(), deferredFramebuffer->GetFramebufferId(),
            0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight(),
            0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        deferredShader->Enable();
        // shader->SetCameraPosition(camera.position);
        testLight.UpdateLightValues(deferredShader.get(), lightUniformLocations);
        gBuffer->ActivateTextures(0);
        deferredShader->SetCameraPosition(currentCamera->GetPosition());
        glBindVertexArray(quad->GetVaoId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef EDITOR
    void Renderer::RenderPickBuffer()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pickBuffer->GetFramebufferId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, pickBuffer->GetFramebufferId());
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        pickShader->Enable();
        pickShader->SetCamera(currentCamera);
        glBindVertexArray(meshVao);

        for (auto& val : uploadedModels | std::views::values)
        {
            val.Render(pickShader.get());
        }

        pickShaderSkeleton->Enable();
        pickShaderSkeleton->SetCamera(currentCamera);
        glBindVertexArray(skeletalMeshVao);

        for (auto& val : uploadedSkeletalModels | std::views::values)
        {
            val.Render(pickShader.get());
        }

        pickShader->Enable();
        glBindVertexArray(meshVao);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (const Ref<MeshInstance>& meshInstance : overlayMeshes)
        {
            meshInstance->GetMeshOwner()->RenderInstance(pickShader.get(), *meshInstance);
        }
    }
#endif
    
    void Renderer::UpdateVoxelMeshes()
    {
        voxelGenerationShader.Enable();
        for (const std::pair<size_t, int>& index : voxelMeshes.GetDiryIndices())
        {
            if (VoxelMesh* mesh = voxelMeshes.Get(index.first, index.second))
            {
                VoxLog(Display, Rendering, "Updating voxel mesh...");
                mesh->Regenerate();
            }
        }
        voxelMeshes.ClearDirty();
    }

    void Renderer::RenderVoxelMeshes()
    {
        glBindVertexArray(voxelMeshVao);
        voxelShader->Enable();
        voxelShader->SetCamera(currentCamera);
        voxelShader->SetArrayTexture(voxelTextures.get());

        for (std::optional<VoxelMesh>& voxelMesh : voxelMeshes)
        {
            if (voxelMesh.has_value())
            {
                RenderVoxelMesh(voxelMesh.value());
            }
        }
    }

    void Renderer::RenderVoxelMesh(VoxelMesh& voxelMesh) const
    {
        glBindVertexBuffer(0, voxelMesh.GetMeshId(), 0, sizeof(float) * 16);
        voxelShader->SetModelMatrix(voxelMesh.GetTransform());
        glDrawArrays(GL_TRIANGLES, 0, 16384);
    }

    void Renderer::RenderDebugShapes()
    {
        PhysicsServer* physicsServer = ServiceLocator::GetPhysicsServer();

        // Fill the debug renderer with our shapes
        physicsServer->RenderDebugShapes();

        glEnable(GL_DEPTH_TEST);
        debugLineShader->Enable();
        physicsServer->GetDebugRenderer()->BindAndBufferLines();
        PixelShader::SetUniformMatrix(debugLineMatrixLocation, currentCamera->GetViewProjectionMatrix());
        glDrawArrays(GL_LINES, 0, static_cast<int>(physicsServer->GetDebugRenderer()->GetLineVertexCount()));

        debugTriangleShader->Enable();
        physicsServer->GetDebugRenderer()->BindAndBufferTriangles();
        PixelShader::SetUniformMatrix(debugTriangleMatrixLocation, currentCamera->GetViewProjectionMatrix());
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(physicsServer->GetDebugRenderer()->GetTriangleVertexCount()));
    }

#ifdef EDITOR
    void Renderer::RenderOutline() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, stencilBuffer->GetFramebufferId());
        glClear(GL_COLOR_BUFFER_BIT);

        stencilShader->Enable();
        stencilShader->SetCamera(currentCamera);
        glBindVertexArray(meshVao);
        for (const Ref<MeshInstance>& mesh : outlinedMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(stencilShader.get(), *mesh);
        }

        stencilShaderSkeleton->Enable();
        stencilShaderSkeleton->SetCamera(currentCamera);
        glBindVertexArray(skeletalMeshVao);
        for (const auto& mesh : outlinedSkeletalMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(stencilShader.get(), *mesh);
        }

        constexpr int outlineWidth = 2;
        glBindVertexArray(quad->GetVaoId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, stencilBuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outlineBuffer->GetFramebufferId());

        outlineShader->Enable();
        stencilBuffer->ActivateTextures(0);
        outlineShader->SetTextureSize(outlineBuffer->GetSize());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        UVec2Buffer* inBuffer = outlineBuffer.get();
        UVec2Buffer* outBuffer = outlineBuffer2.get();

        for (unsigned int floodWidth = NextPowerOfTwo(outlineWidth); floodWidth; floodWidth = floodWidth >> 1)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, inBuffer->GetFramebufferId());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outBuffer->GetFramebufferId());

            outlineShaderJump->Enable();
            outlineShaderJump->SetWidth(static_cast<int>(floodWidth));
            inBuffer->ActivateTextures(0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            std::swap(inBuffer, outBuffer);
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, inBuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());

        outlineShaderDistance->Enable();
        outlineShaderDistance->SetOutlineColor({1.0f, 0.039f, 0.4f});
        inBuffer->ActivateTextures(0);
        deferredFramebuffer->ActivateTextures(1);
        outlineShaderDistance->SetWidth(outlineWidth);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void Renderer::RenderOverlay() const
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(meshVao);
        overlayShader->Enable();
        overlayShader->SetCamera(currentCamera);
        for (const Ref<MeshInstance>& mesh : overlayMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(overlayShader.get(), *mesh);
        }
    }
#endif

    void Renderer::RenderSky()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());

        skyShader->Enable();
        const glm::mat4x4 cameraRotation = currentCamera->GetRotationMatrix();
        const glm::mat4x4 projection = currentCamera->GetProjectionMatrix();
        const glm::mat4x4 matrix = glm::inverse(projection * cameraRotation);
        PixelShader::SetUniformMatrix(skyShader->GetUniformLocation("cameraWorldSpace"), matrix);
        // deferredFramebuffer->ActivateTextures();
        glBindVertexArray(quad->GetVaoId());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
}
