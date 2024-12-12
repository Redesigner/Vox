#pragma once

#include "Renderer.h"

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"

#include "core/services/ServiceLocator.h"
#include "editor/Editor.h"
#include "external/glad.h"
#include "rendering/ArrayTexture.h"
#include "rendering/Camera.h"
#include "rendering/DebugRenderer.h"
#include "rendering/Framebuffer.h"
#include "rendering/GBuffer.h"
#include "rendering/shaders/DeferredShader.h"
#include "rendering/shaders/VoxelShader.h"
#include "physics/PhysicsServer.h"
#include "voxel/Octree.h"
#include "voxel/VoxelGrid.h"

Vox::Renderer::Renderer()
{
    // This is realtive to the build location -- I'll have to fix this later
    ChangeDirectory("../../../");

    TraceLog(LOG_INFO, TextFormat("Set current working directory to '%s'", GetWorkingDirectory()));
    gBufferShader = LoadShader("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
    materialRoughnessLocation = GetShaderLocation(gBufferShader, "materialRoughness");
    materialColorLocation = GetShaderLocation(gBufferShader, "materialAlbedo");
    voxelShader = std::make_unique<VoxelShader>();

    deferredShader = std::make_unique<DeferredShader>();
    skyShader = LoadShader("assets/shaders/sky.vert", "assets/shaders/sky.frag");
    debugLineShader = LoadShader("assets/shaders/debugLine.vert", "assets/shaders/debugLine.frag");

    debugMatrixLocation = GetShaderLocation(debugLineShader, "viewProjection");

    gBuffer = std::make_unique<GBuffer>(800, 431);
    deferredFramebuffer = std::make_unique<Framebuffer>(800, 431);

    defaultMaterial = LoadMaterialDefault();

    rlEnableShader(skyShader.id);
    {
        int position = 0;
        rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "color"), position);
        rlSetUniform(GetShaderLocation(skyShader, "color"), &position, SHADER_UNIFORM_INT, 1);

        position = 1;
        rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "depth"), position);
        rlSetUniform(GetShaderLocation(skyShader, "depth"), &position, SHADER_UNIFORM_INT, 1);
        rlDisableShader();
    }
    defaultMaterial.maps[MATERIAL_MAP_DIFFUSE].color = GRAY;
    defaultMaterial.shader = gBufferShader;

    //int dataSize = 0;
    //unsigned char* octreeRaw = LoadFileData("assets/voxels/test.vox", &dataSize);
    //std::vector<char> octreeData;
    //octreeData.insert(octreeData.begin(), octreeRaw, octreeRaw + dataSize);
    //std::shared_ptr<Octree::Node> octree = Octree::Node::FromPacked(octreeData);
    //testVoxelGrid = VoxelGrid::FromOctree(octree.get());
    testVoxelGrid = std::make_shared<VoxelGrid>(32, 32, 32);
    for (int x = 0; x < 32; ++x)
    {
        for (int y = 0; y < 16; ++y)
        {
            for (int z = 0; z < 32; ++z)
            {
                testVoxelGrid->GetVoxel(x, y, z).filled = true;
            }
        }
    }

    testVoxelGrid->x = -16;
    testVoxelGrid->y = -16;
    testVoxelGrid->z = -16;
    testVoxelGrid->GenerateMesh();

    voxelTextures = std::make_unique<ArrayTexture>(64, 64, 5, 1);
    voxelTextures->LoadTexture("assets/textures/voxel0.png", 0);
    voxelTextures->LoadTexture("assets/textures/voxel1.png", 1);

    camera = std::make_unique<Vox::Camera>();
    camera->SetPosition(6.0f, 2.0f, 6.0f);
    camera->SetRotation(0.0f, 0.0f, 0.0f);
    camera->SetFovY(45.0f);
    camera->SetAspectRatio(800.0 / 431.0);

    Camera* movedCamera = camera.get();

    lightUniformLocations = LightUniformLocations(deferredShader.get());
    testLight = Light(1, 1, Vector3(4.5f, 4.5f, 0.5f), Vector3(), Vector4(255.0f, 255.0f, 255.0f, 255.0f), 1000.0f);

    viewportTexture = RenderTexture2D();
    testModel = LoadModel("assets/models/mushroom.glb");
    for (int i = 0; i < testModel.materialCount; ++i)
    {
        testModel.materials[i].shader = gBufferShader;
    }
}

Vox::Renderer::~Renderer()
{
    if (IsRenderTextureValid(viewportTexture))
    {
        UnloadRenderTexture(viewportTexture);
    }

    if (IsModelValid(testModel))
    {
        UnloadModel(testModel);
    }

    // Unload shaders
    // @TODO: do this in a shader C++ wrapper
    if (IsShaderValid(gBufferShader))
    {
        UnloadShader(gBufferShader);
    }
    if (IsShaderValid(skyShader))
    {
        UnloadShader(skyShader);
    }
}

void Vox::Renderer::Render(Editor* editor)
{
    SwapScreenBuffer();
    UpdateViewportDimensions(editor);

    BeginDrawing();
    {
        camera->SetAspectRatio(viewportTexture.texture.height == 0 ? 1 : viewportTexture.texture.width / viewportTexture.texture.height);
        //camera->SetRotation(camera->GetRotation() + Vector3(0.0f, 0.01f, 0.0f));

        rlViewport(0, 0, viewportTexture.texture.width, viewportTexture.texture.height);
        rlSetFramebufferWidth(viewportTexture.texture.width);
        rlSetFramebufferHeight(viewportTexture.texture.height);

        RenderGBuffer();
        // RenderVoxelGrid(testVoxelGrid.get());
        RenderDeferred();
        RenderSky();

        // Copy into texture to be rendered with imgui
        deferredFramebuffer->BindRead();
        rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, viewportTexture.id);
        rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00004000 | 0x00000100);
        RenderDebugShapes();
        rlDisableFramebuffer();

        // Make sure that our viewport size matches the window size when drawing with imgui
        rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
        rlSetFramebufferWidth(GetScreenWidth());
        rlSetFramebufferHeight(GetScreenHeight());
        editor->Draw(&viewportTexture);
        EndDrawing();
    }
}

void Vox::Renderer::LoadTestModel(std::string path)
{
    if (IsModelValid(testModel))
    {
        UnloadModel(testModel);
    }
    testModel = LoadModel(path.c_str());
    for (int i = 0; i < testModel.materialCount; ++i)
    {
        testModel.materials[i].shader = gBufferShader;
    }
}

void Vox::Renderer::SetDebugPhysicsServer(std::shared_ptr<PhysicsServer> physicsServer)
{
    debugPhysicsServer = physicsServer;
}

void Vox::Renderer::SetCameraPosition(Vector3 position)
{
    camera->SetPosition(position);
}

void Vox::Renderer::SetCameraRotation(Vector3 rotation)
{
    camera->SetRotation(rotation);
}

void Vox::Renderer::UpdateViewportDimensions(Editor* editor)
{
    // Resize our render texture if it's the wrong size, so we get a 1:1 resolution for the editor viewport
    Vector2 editorViewportSize = editor->GetViewportDimensions();
    if (IsRenderTextureValid(viewportTexture))
    {
        if (viewportTexture.texture.width != static_cast<int>(editorViewportSize.x) || viewportTexture.texture.height != static_cast<int>(editorViewportSize.y))
        {
            UnloadRenderTexture(viewportTexture);
            viewportTexture = LoadRenderTexture(editorViewportSize.x, editorViewportSize.y);

            // @TODO: add resize method?
            gBuffer.reset();
            gBuffer = std::make_unique<GBuffer>(editorViewportSize.x, editorViewportSize.y);
            deferredFramebuffer.reset();
            deferredFramebuffer = std::make_unique<Framebuffer>(editorViewportSize.x, editorViewportSize.y);
            rlViewport(0, 0, viewportTexture.texture.width, viewportTexture.texture.height);
        }
    }
    else
    {
        viewportTexture = LoadRenderTexture(editorViewportSize.x, editorViewportSize.y);
    }
}

void Vox::Renderer::RenderGBuffer()
{
    ClearBackground(BLACK);
    gBuffer->EnableFramebuffer();
    rlClearScreenBuffers();
    rlDisableColorBlend();

    glUseProgram(gBufferShader.id);
    {
        // Set camera matrices
        SetShaderValueMatrix(gBufferShader, gBufferShader.locs[SHADER_LOC_MATRIX_VIEW], camera->GetViewMatrix());
        SetShaderValueMatrix(gBufferShader, gBufferShader.locs[SHADER_LOC_MATRIX_PROJECTION], camera->GetProjectionMatrix());

        if (IsModelValid(testModel))
        {
            for (int i = 0; i < testModel.meshCount; ++i)
            {
                DrawMeshGBuffer(&testModel.meshes[i], &testModel.materials[testModel.meshMaterial[i]], testModel.transform);
            }
        }
    }
    rlEnableColorBlend();
}

void Vox::Renderer::DrawMeshGBuffer(Mesh* mesh, Material* material, const Matrix& transform)
{
    const Color albedo = material->maps[MATERIAL_MAP_DIFFUSE].color;
    Vector3 materialAlbedo = Vector3(
        albedo.r / 255.0f,
        albedo.g / 255.0f,
        albedo.b / 255.0f
    );
    glUniform3fv(materialColorLocation, 1, reinterpret_cast<float*>(&materialAlbedo));
    // The shader uniform location is automatically assigned when the shader is loaded by raylib
    // SetShaderValueMatrix(gBufferShader, gBufferShader.locs[SHADER_LOC_MATRIX_MODEL], transform);
    rlSetUniformMatrix(gBufferShader.locs[SHADER_LOC_MATRIX_MODEL], transform);

    glBindVertexArray(mesh->vaoId);
    glDrawElements(GL_TRIANGLES, mesh->triangleCount * 3, GL_UNSIGNED_SHORT, 0);
}

void Vox::Renderer::RenderDeferred()
{
    gBuffer->BindRead();
    deferredFramebuffer->BindDraw();

    rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00000100);

    rlDisableColorBlend();
    rlDisableDepthMask();
    deferredShader->Enable();
    {
        // shader->SetCameraPosition(camera.position);
        testLight.UpdateLightValues(deferredShader.get(), lightUniformLocations);
        gBuffer->ActivateTextures(0);
        deferredShader->SetCameraPosition(camera->GetPosition());
        rlLoadDrawQuad();
        rlDisableShader();
    }
    // rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00000100);
    rlEnableColorBlend();
    rlEnableDepthMask();
}

void Vox::Renderer::RenderVoxelGrid(VoxelGrid* voxelGrid)
{
    rlDisableColorBlend();
    rlEnableDepthTest();
    voxelShader->Enable();
    voxelShader->SetModelMatrix(MatrixTranslate(voxelGrid->x, voxelGrid->y, voxelGrid->z) * MatrixScale(2.0f, 2.0f, 2.0f));
    voxelShader->SetViewMatrix(camera->GetViewMatrix());
    voxelShader->SetProjectionMatrix(camera->GetProjectionMatrix());
    voxelGrid->EnableVertexArray();
    voxelShader->SetArrayTexture(voxelTextures.get());
    // rlDrawVertexArrayElements(0, voxelGrid->GetVertexCount(), 0);
    glDrawElements(GL_TRIANGLES, voxelGrid->GetVertexCount(), GL_UNSIGNED_INT, 0);
    rlDrawRenderBatchActive();
    rlDisableVertexArray();
    rlDisableDepthTest();
}

void Vox::Renderer::RenderDebugShapes()
{
    std::shared_ptr<Vox::PhysicsServer> physicsServer = debugPhysicsServer.lock();
    if (!physicsServer)
    {
        return;
    }

    // Fill the debug renderer with our shapes
    physicsServer->RenderDebugShapes();

    physicsServer->GetDebugRenderer()->BindAndBufferLines();
    rlEnableShader(debugLineShader.id);
    rlSetUniformMatrix(debugMatrixLocation, camera->GetViewProjectionMatrix());
    glDrawArrays(GL_LINES, 0, physicsServer->GetDebugRenderer()->GetLineVertexCount());

    physicsServer->GetDebugRenderer()->BindAndBufferTriangles();
    rlSetUniformMatrix(debugMatrixLocation, camera->GetViewProjectionMatrix());
    glDrawArrays(GL_TRIANGLES, 0, physicsServer->GetDebugRenderer()->GetTriangleVertexCount());
}

void Vox::Renderer::RenderSky()
{
    rlDisableDepthMask();
    deferredFramebuffer->BindRead();
    deferredFramebuffer->BindDraw();

    rlEnableDepthTest();

    rlEnableShader(skyShader.id);
    {
        float aspect = static_cast<float>(viewportTexture.texture.width) / static_cast<float>(viewportTexture.texture.height);
        Matrix cameraRotation = camera->GetRotationMatrix();;
        Matrix projection = camera->GetProjectionMatrix();
        Matrix matrix = MatrixInvert(cameraRotation * projection);
        rlSetUniformMatrix(rlGetLocationUniform(skyShader.id, "cameraWorldSpace"), matrix);
        deferredFramebuffer->ActivateTextures();
        rlLoadDrawQuad();
        rlDisableFramebuffer();
        rlDisableShader();
    }
    rlEnableDepthMask();
}

void Vox::Renderer::CopyViewportToTexture(RenderTexture2D& texture)
{
    rlBindFramebuffer(RL_READ_FRAMEBUFFER, 0);
    rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, viewportTexture.id);
    rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00004000 | 0x00000100);
    rlDisableFramebuffer();
}
