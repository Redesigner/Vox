#include "WorldRenderer.h"

#include <ranges>
#include <GL/glew.h>

#include "Camera.h"
#include "DebugRenderer.h"
#include "core/math/Math.h"
#include "core/services/ServiceLocator.h"
#include "physics/PhysicsServer.h"
#include "rendering/Renderer.h"
#include "rendering/buffers/frame_buffers/ColorDepthFramebuffer.h"
#include "rendering/buffers/frame_buffers/GBuffer.h"
#include "rendering/buffers/frame_buffers/PickBuffer.h"
#include "rendering/buffers/frame_buffers/StencilBuffer.h"
#include "rendering/buffers/frame_buffers/UVec2Buffer.h"
#include "rendering/skeletal_mesh/SkeletalMeshInstanceContainer.h"
#include "shaders/pixel_shaders/DebugShader.h"
#include "shaders/pixel_shaders/DeferredShader.h"
#include "shaders/pixel_shaders/SkyShader.h"
#include "shaders/pixel_shaders/mesh_shaders/PickShader.h"
#include "shaders/pixel_shaders/mesh_shaders/StencilShader.h"
#include "shaders/pixel_shaders/mesh_shaders/VoxelShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShader.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderDistance.h"
#include "shaders/pixel_shaders/outline_shaders/OutlineShaderJump.h"

namespace Vox
{
    WorldRenderer::WorldRenderer()
    {
        GenerateBuffers();
        lightUniformLocations = LightUniformLocations(GetRenderer()->GetDeferredShader());
    }

    void WorldRenderer::Draw()
    {
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        DrawGBuffer();
        DrawDeferredPass();
        DrawSky();

#ifdef EDITOR
        DrawPickBuffer();
        DrawOutline();
        DrawOverlay();
#endif
    }

    void WorldRenderer::DrawGBuffer()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        const MaterialShader* gBufferShader = GetRenderer()->GetGBufferShader();
        gBufferShader->Enable();
        gBufferShader->SetCamera(currentCamera);
        GetRenderer()->BindMeshVao();

        for (auto& val : meshInstances | std::views::values)
        {
            val.Render(gBufferShader);
        }

        MaterialShader* skeletonShader = GetRenderer()->GetGBufferShaderSkeleton();
        skeletonShader->Enable();
        skeletonShader->SetCamera(currentCamera);
        GetRenderer()->BindSkeletalMeshVao();

        for (SkeletalMeshInstanceContainer& val : skeletalMeshInstances | std::views::values)
        {
            val.Render(skeletonShader);
        }

        UpdateVoxels();
        DrawVoxels();
    }

    void WorldRenderer::DrawDeferredPass()
    {
        glBlitNamedFramebuffer(gBuffer->GetFramebufferId(), deferredFramebuffer->GetFramebufferId(),
            0, 0, gBuffer->GetWidth(), deferredFramebuffer->GetHeight(),
            0, 0, gBuffer->GetWidth(), deferredFramebuffer->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        DeferredShader* deferredShader = GetRenderer()->GetDeferredShader();
        deferredShader->Enable();
        // shader->SetCameraPosition(camera.position);
        testLight.UpdateLightValues(deferredShader, lightUniformLocations);
        gBuffer->ActivateTextures(0);
        deferredShader->SetCameraPosition(currentCamera->GetPosition());
        GetRenderer()->BindQuadVao();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef EDITOR
    void WorldRenderer::DrawPickBuffer()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pickBuffer->GetFramebufferId());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, pickBuffer->GetFramebufferId());
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        const PickShader* pickShader = GetRenderer()->GetPickShader();
        pickShader->Enable();
        pickShader->SetCamera(currentCamera);
        GetRenderer()->BindMeshVao();

        for (auto& val : meshInstances | std::views::values)
        {
            val.Render(pickShader);
        }

        const PickShader* pickShaderSkeleton = GetRenderer()->GetPickShaderSkeleton();
        pickShaderSkeleton->Enable();
        pickShaderSkeleton->SetCamera(currentCamera);
        GetRenderer()->BindSkeletalMeshVao();

        for (auto& val : skeletalMeshInstances | std::views::values)
        {
            val.Render(pickShaderSkeleton);
        }

        pickShader->Enable();
        GetRenderer()->BindMeshVao();
        glClear(GL_DEPTH_BUFFER_BIT);
        for (const Ref<MeshInstance>& meshInstance : overlayMeshes)
        {
            meshInstance->GetMeshOwner()->RenderInstance(pickShader, *meshInstance);
        }
    }

    void WorldRenderer::DrawOutline() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, stencilBuffer->GetFramebufferId());
        glClear(GL_COLOR_BUFFER_BIT);

        const StencilShader* stencilShader = GetRenderer()->GetStencilShader();
        stencilShader->Enable();
        stencilShader->SetCamera(currentCamera);
        GetRenderer()->BindMeshVao();
        for (const Ref<MeshInstance>& mesh : outlinedMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(stencilShader, *mesh);
        }

        const StencilShader* stencilShaderSkeleton = GetRenderer()->GetStencilShaderSkeleton();
        stencilShaderSkeleton->Enable();
        stencilShaderSkeleton->SetCamera(currentCamera);
        GetRenderer()->BindSkeletalMeshVao();
        for (const auto& mesh : outlinedSkeletalMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(stencilShader, *mesh);
        }

        constexpr int outlineWidth = 2;
        GetRenderer()->BindQuadVao();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, stencilBuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outlineBuffer->GetFramebufferId());

        const OutlineShader* outlineShader = GetRenderer()->GetOutlineShader();
        outlineShader->Enable();
        stencilBuffer->ActivateTextures(0);
        outlineShader->SetTextureSize(outlineBuffer->GetSize());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        UVec2Buffer* inBuffer = outlineBuffer.get();
        UVec2Buffer* outBuffer = outlineBuffer2.get();

        const OutlineShaderJump* outlineShaderJump = GetRenderer()->GetOutlineShaderJump();
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

        const OutlineShaderDistance* outlineShaderDistance = GetRenderer()->GetOutlineShaderDistance();
        outlineShaderDistance->Enable();
        outlineShaderDistance->SetOutlineColor({1.0f, 0.039f, 0.4f});
        inBuffer->ActivateTextures(0);
        deferredFramebuffer->ActivateTextures(1);
        outlineShaderDistance->SetWidth(outlineWidth);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void WorldRenderer::DrawOverlay() const
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GetRenderer()->BindMeshVao();
        const MaterialShader* overlayShader = GetRenderer()->GetOverlayShader();
        overlayShader->Enable();
        overlayShader->SetCamera(currentCamera);
        for (const Ref<MeshInstance>& mesh : overlayMeshes)
        {
            mesh->GetMeshOwner()->RenderInstance(overlayShader, *mesh);
        }
    }
#endif

    void WorldRenderer::DrawVoxels()
    {
        GetRenderer()->BindVoxelMeshVao();
        const VoxelShader* voxelShader = GetRenderer()->GetVoxelMeshShader();
        voxelShader->Enable();
        voxelShader->SetCamera(currentCamera);
        voxelShader->SetArrayTexture(GetRenderer()->GetVoxelTexture());

        for (std::optional<VoxelMesh>& voxelMesh : voxelMeshes)
        {
            if (voxelMesh.has_value())
            {
                glBindVertexBuffer(0, voxelMesh->GetMeshId(), 0, sizeof(float) * 16);
                voxelShader->SetModelMatrix(voxelMesh->GetTransform());
                glDrawArrays(GL_TRIANGLES, 0, 16384);
            }
        }
    }

    void WorldRenderer::DrawSky() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());

        const SkyShader* skyShader = GetRenderer()->GetSkyShader();
        skyShader->Enable();
        skyShader->SetCamera(currentCamera);
        GetRenderer()->BindQuadVao();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void WorldRenderer::DrawDebugShapes() const
    {
        PhysicsServer* physicsServer = ServiceLocator::GetPhysicsServer();

        // Fill the debug renderer with our shapes
        physicsServer->RenderDebugShapes();

        glEnable(GL_DEPTH_TEST);

        DebugRenderer* debugRenderer = ServiceLocator::GetPhysicsServer()->GetDebugRenderer();

        const DebugShader* debugLineShader = GetRenderer()->GetDebugLineShader();
        debugRenderer->BindAndBufferLines();
        debugLineShader->Enable();
        debugLineShader->SetCamera(currentCamera);
        glDrawArrays(GL_LINES, 0, static_cast<int>(debugRenderer->GetLineVertexCount()));

        const DebugShader* debugTriangleShader = GetRenderer()->GetDebugTriangleShader();
        physicsServer->GetDebugRenderer()->BindAndBufferTriangles();
        debugTriangleShader->Enable();
        debugTriangleShader->SetCamera(currentCamera);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(physicsServer->GetDebugRenderer()->GetTriangleVertexCount()));
    }

    void WorldRenderer::UpdateVoxels()
    {
        GetRenderer()->GetVoxelGenerationShader()->Enable();
        for (const auto& [index, snd] : voxelMeshes.GetDiryIndices())
        {
            if (VoxelMesh* mesh = voxelMeshes.Get(index, snd))
            {
                mesh->Regenerate();
            }
        }
        voxelMeshes.ClearDirty();
    }

    void WorldRenderer::GenerateBuffers()
    {
        constexpr int defaultWidth = 800;
        constexpr int defaultHeight = 450;

        gBuffer = std::make_unique<GBuffer>(defaultWidth, defaultHeight);
        deferredFramebuffer = std::make_unique<ColorDepthFramebuffer>(defaultWidth, defaultHeight);

#ifdef EDITOR
        pickBuffer = std::make_unique<PickBuffer>(defaultWidth, defaultHeight);
        stencilBuffer = std::make_unique<StencilBuffer>(defaultWidth, defaultHeight);
        outlineBuffer = std::make_unique<UVec2Buffer>(defaultWidth, defaultHeight);
        outlineBuffer2 = std::make_unique<UVec2Buffer>(defaultWidth, defaultHeight);
#endif
    }

    void WorldRenderer::ConditionalResizeFramebuffers()
    {
        if (gBuffer->GetWidth() != viewportSize.x || gBuffer->GetHeight() != viewportSize.y)
        {
            const int width = viewportSize.x;
            const int height = viewportSize.y;
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
    }

    Renderer* WorldRenderer::GetRenderer() const
    {
        return ServiceLocator::GetRenderer();
    }
} // Vox