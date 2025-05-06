#include "Renderer.h"

#include <ranges>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <utility>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_video.h>

#include "core/logging/Logging.h"
#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "rendering/ArrayTexture.h"
#include "rendering/Camera.h"
#include "rendering/DebugRenderer.h"
#include "rendering/Framebuffer.h"
#include "rendering/FullscreenQuad.h"
#include "rendering/GBuffer.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/RenderTexture.h"
#include "rendering/shaders/DeferredShader.h"
#include "rendering/shaders/VoxelShader.h"
#include "physics/PhysicsServer.h"

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

        gBufferShader.Load("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
        gBufferModelMatrixLocation = gBufferShader.GetUniformLocation("matModel");
        gBufferViewMatrixLocation = gBufferShader.GetUniformLocation("matView");
        gBufferProjectionMatrixLocation = gBufferShader.GetUniformLocation("matProjection");
        gBufferRoughnessLocation = gBufferShader.GetUniformLocation("materialRoughness");
        gBufferAlbedoLocation = gBufferShader.GetUniformLocation("materialAlbedo");

        skeletalMeshShader.Load("assets/shaders/skeletalMesh.vert", "assets/shaders/gBuffer.frag");
        skeletalModelMatrixLocation = skeletalMeshShader.GetUniformLocation("matModel");
        skeletalViewMatrixLocation = skeletalMeshShader.GetUniformLocation("matView");
        skeletalProjectionMatrixLocation = skeletalMeshShader.GetUniformLocation("matProjection");
        skeletalRoughnessLocation = skeletalMeshShader.GetUniformLocation("materialRoughness");
        skeletalAlbedoLocation = skeletalMeshShader.GetUniformLocation("materialAlbedo");
        unsigned int matrixBufferLocation = glGetUniformBlockIndex(skeletalMeshShader.GetId(), "data");
        glUniformBlockBinding(skeletalMeshShader.GetId(), matrixBufferLocation, 0);

        voxelShader = std::make_unique<VoxelShader>();
        deferredShader = std::make_unique<DeferredShader>();

        skyShader.Load("assets/shaders/sky.vert", "assets/shaders/sky.frag");
        debugTriangleShader.Load("assets/shaders/debugTriangle.vert", "assets/shaders/debugTriangle.frag");
        debugLineShader.Load("assets/shaders/debugLine.vert", "assets/shaders/debugLine.frag");

        debugTriangleMatrixLocation = debugTriangleShader.GetUniformLocation("viewProjection");
        debugLineMatrixLocation = debugLineShader.GetUniformLocation("viewProjection");

        voxelGenerationShader.Load("assets/shaders/voxelGeneration.comp");

        CreateMeshVao();
        CreateSkeletalMeshVao();
        CreateVoxelVao();

        gBuffer = std::make_unique<GBuffer>(800, 450);
        deferredFramebuffer = std::make_unique<Framebuffer>(800, 450);

        skyShader.Enable();
        Vox::PixelShader::SetUniformInt(skyShader.GetUniformLocation("color"), 0);

        Vox::PixelShader::SetUniformInt(skyShader.GetUniformLocation("depth"), 1);

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
        UpdateViewportDimensions(editor);
        currentCamera->SetAspectRatio(viewportTexture->GetHeight() == 0 ? 1 : viewportTexture->GetWidth() / viewportTexture->GetHeight());
        glViewport(0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight());

        RenderGBuffer();
        RenderDeferred();
        RenderSky();
        // RenderDebugShapes();

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

    //void Renderer::LoadTestModel(std::string path)
    //{
    //    if (IsModelValid(testModel))
    //    {
    //        UnloadModel(testModel);
    //    }
    //    testModel = LoadModel(path.c_str());
    //    for (int i = 0; i < testModel.materialCount; ++i)
    //    {
    //        testModel.materials[i].shader = gBufferShader;
    //    }
    //}

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

        uploadedSkeletalModels.emplace(alias, relativeFilePath);
        return true;
    }

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

    const std::unordered_map<std::string, SkeletalModel>& Renderer::GetSkeletalMeshes() const
    {
        return uploadedSkeletalModels;
    }

    void Renderer::UpdateViewportDimensions(const Editor* editor)
    {
        // Resize our render texture if it's the wrong size, so we get a 1:1 resolution for the editor viewport
        glm::vec2 editorViewportSize = editor->GetViewportDimensions();
        if (viewportTexture)
        {
            int viewportWidth = static_cast<int>(editorViewportSize.x);
            int viewportHeight = static_cast<int>(editorViewportSize.y);
            if (viewportTexture->GetWidth() != viewportWidth || viewportTexture->GetHeight() != viewportHeight)
            {
                VoxLog(Display, Rendering, "Resizing GBuffer and Viewport texture to '({}, {})'", viewportWidth, viewportHeight);
                viewportTexture.reset();
                viewportTexture = std::make_unique<RenderTexture>(viewportWidth, viewportHeight);

                // @TODO: add resize method?
                gBuffer.reset();
                gBuffer = std::make_unique<GBuffer>(viewportWidth, viewportHeight);
                deferredFramebuffer.reset();
                deferredFramebuffer = std::make_unique<Framebuffer>(viewportWidth, viewportHeight);
                glViewport(0, 0, viewportWidth, viewportHeight);
            }
        }
        else
        {
            // viewportTexture = std::make_unique<RenderTexture>(static_cast<int>(editorViewportSize.x), static_cast<int>(editorViewportSize.y));
            viewportTexture = std::make_unique<RenderTexture>(800, 450);
        }
    }

    void Renderer::RenderGBuffer()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClearDepth(0.0f);

        gBufferShader.Enable();
        Vox::PixelShader::SetUniformMatrix(gBufferViewMatrixLocation, currentCamera->GetViewMatrix());
        Vox::PixelShader::SetUniformMatrix(gBufferProjectionMatrixLocation, currentCamera->GetProjectionMatrix());
        glBindVertexArray(meshVao);

        for (auto& val : uploadedModels | std::views::values)
        {
            val.Render(gBufferShader, gBufferModelMatrixLocation, gBufferAlbedoLocation, gBufferRoughnessLocation);
        }

        skeletalMeshShader.Enable();
        Vox::PixelShader::SetUniformMatrix(skeletalViewMatrixLocation, currentCamera->GetViewMatrix());
        Vox::PixelShader::SetUniformMatrix(skeletalProjectionMatrixLocation, currentCamera->GetProjectionMatrix());
        glBindVertexArray(skeletalMeshVao);

        for (auto& val : uploadedSkeletalModels | std::views::values)
        {
            val.Render(skeletalMeshShader, skeletalModelMatrixLocation, glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.0f, 1.0f, 0.0f)), skeletalAlbedoLocation, skeletalRoughnessLocation);
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
        voxelShader->SetViewMatrix(currentCamera->GetViewMatrix());
        voxelShader->SetProjectionMatrix(currentCamera->GetProjectionMatrix());
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
        debugLineShader.Enable();
        physicsServer->GetDebugRenderer()->BindAndBufferLines();
        PixelShader::SetUniformMatrix(debugLineMatrixLocation, currentCamera->GetViewProjectionMatrix());
        glDrawArrays(GL_LINES, 0, static_cast<int>(physicsServer->GetDebugRenderer()->GetLineVertexCount()));

        debugTriangleShader.Enable();
        physicsServer->GetDebugRenderer()->BindAndBufferTriangles();
        PixelShader::SetUniformMatrix(debugTriangleMatrixLocation, currentCamera->GetViewProjectionMatrix());
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(physicsServer->GetDebugRenderer()->GetTriangleVertexCount()));
    }

    void Renderer::RenderSky()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());

        skyShader.Enable();
        const glm::mat4x4 cameraRotation = currentCamera->GetRotationMatrix();
        const glm::mat4x4 projection = currentCamera->GetProjectionMatrix();
        const glm::mat4x4 matrix = glm::inverse(projection * cameraRotation);
        PixelShader::SetUniformMatrix(skyShader.GetUniformLocation("cameraWorldSpace"), matrix);
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