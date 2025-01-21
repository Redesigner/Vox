#pragma once

#include "Renderer.h"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
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
#include "rendering/RenderTexture.h"
#include "rendering/shaders/DeferredShader.h"
#include "rendering/shaders/VoxelShader.h"
#include "physics/PhysicsServer.h"
#include "voxel/Octree.h"
#include "voxel/VoxelGrid.h"

Vox::Renderer::Renderer(SDL_Window* window)
{
    mainWindow = window;
    // This is realtive to the build location -- I'll have to fix this later
    // ChangeDirectory("../../../");

    VoxLog(Display, Rendering, "Set current working directory to {}", SDL_GetBasePath());

    quad = std::make_unique<FullscreenQuad>();

    gBufferShader.Load("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
    gBufferModelMatrixLocation = gBufferShader.GetUniformLocation("matModel");
    gBufferViewMatrixLocation = gBufferShader.GetUniformLocation("matView");
    gBufferProjectionMatrixLocation = gBufferShader.GetUniformLocation("matProjection");
    materialRoughnessLocation = gBufferShader.GetUniformLocation("materialRoughness");
    materialColorLocation = gBufferShader.GetUniformLocation("materialAlbedo");

    voxelShader = std::make_unique<VoxelShader>();
    deferredShader = std::make_unique<DeferredShader>();

    skyShader.Load("assets/shaders/sky.vert", "assets/shaders/sky.frag");
    debugTriangleShader.Load("assets/shaders/debugTriangle.vert", "assets/shaders/debugTriangle.frag");
    debugLineShader.Load("assets/shaders/debugLine.vert", "assets/shaders/debugLine.frag");

    debugTriangleMatrixLocation = debugTriangleShader.GetUniformLocation("viewProjection");
    debugLineMatrixLocation = debugLineShader.GetUniformLocation("viewProjection");

    voxelGenerationShader.Load("assets/shaders/voxelGeneration.comp");

    const unsigned int voxelGridSize = 34;

    unsigned int voxelData[voxelGridSize][voxelGridSize][voxelGridSize] = { 0 };
    voxelData[0][0][1] = 1;
    voxelData[0][0][0] = 1;
    voxelData[0][0][2] = 1;
    voxelData[0][0][3] = 1;
    voxelData[1][0][1] = 1;
    voxelData[16][18][16] = 1;
    voxelData[3][3][2] = 1;
    voxelData[1][1][1] = 1;
    voxelData[31][31][31] = 1;

    for (int x = 1; x < 33; ++x)
    {
        for (int z = 1; z < 33; ++z)
        {
            if (rand() % 5 != 0)
            {
                voxelData[x][5][z] = 1;
            }
        }
    }

    unsigned int voxelBuffers[2];
    glCreateBuffers(2, voxelBuffers);
    voxelSsbo = voxelBuffers[0];
    voxelMeshSsbo = voxelBuffers[1];

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(voxelData), voxelData, GL_STREAM_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxelSsbo);

    size_t voxelMeshStride = sizeof(float) * 16;
    glNamedBufferStorage(voxelMeshSsbo, voxelMeshStride * 1024 * 6, NULL, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelMeshSsbo);

    unsigned int vertexCounterSsbo = 0;
    glGenBuffers(1, &vertexCounterSsbo);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, vertexCounterSsbo);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), NULL, GL_DYNAMIC_READ);
    GLuint* a = { 0 };
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), a);
    //glNamedBufferStorage(vertexCounterSsbo, sizeof(int), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, vertexCounterSsbo);

    voxelMeshVao = 0;
    glGenVertexArrays(1, &voxelMeshVao);
    glBindVertexArray(voxelMeshVao);
    glBindBuffer(GL_ARRAY_BUFFER, voxelMeshSsbo);
    size_t texCoordOffset = sizeof(float) * 4;
    size_t normalOffest = sizeof(float) * 8;
    size_t materialIdOffset = sizeof(float) * 12;
    glVertexAttribPointer(0, 3, GL_FLOAT, false, voxelMeshStride, 0); // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, voxelMeshStride, reinterpret_cast<void*>(texCoordOffset)); // texCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, voxelMeshStride, reinterpret_cast<void*>(normalOffest)); // normal
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, voxelMeshStride, reinterpret_cast<void*>(materialIdOffset)); // texCoord
    glEnableVertexAttribArray(3);

    voxelGenerationShader.Enable();
    glDispatchCompute(1, 1, 1);

    gBuffer = std::make_unique<GBuffer>(800, 450);
    deferredFramebuffer = std::make_unique<Framebuffer>(800, 450);

    skyShader.Enable();
    skyShader.SetUniformInt(skyShader.GetUniformLocation("color"), 0);
    //int position = 0;
    //rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "color"), position);
    //rlSetUniform(GetShaderLocation(skyShader, "color"), &position, SHADER_UNIFORM_INT, 1);

    skyShader.SetUniformInt(skyShader.GetUniformLocation("depth"), 1);
    //position = 1;
    //rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "depth"), position);
    //rlSetUniform(GetShaderLocation(skyShader, "depth"), &position, SHADER_UNIFORM_INT, 1);
    //rlDisableShader();

    //defaultMaterial.maps[MATERIAL_MAP_DIFFUSE].color = GRAY;
    //defaultMaterial.shader = gBufferShader;

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
    testVoxelGrid->GetVoxel(21, 18, 16).filled = true;
    testVoxelGrid->GetVoxel(18, 17, 16).filled = true;
    testVoxelGrid->GetVoxel(18, 17, 17).filled = true;
    testVoxelGrid->GetVoxel(19, 17, 18).filled = true;

    //srand(5);
    //for (int x = 0; x < 5; ++x)
    //{
    //    for (int z = 0; z < 5; ++z)
    //    {
    //        if (rand() % 5 != 0)
    //        {
    //            testVoxelGrid->GetVoxel(x, 16, z).filled = true;
    //        }
    //    }
    //}

    //testVoxelGrid->GenerateSlice(16);
    //testVoxelGrid->GenerateSlice(17);

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
    camera->SetAspectRatio(800.0 / 431.0);
    camera->SetFovY(45.0f);
    Camera* movedCamera = camera.get();

    lightUniformLocations = LightUniformLocations(deferredShader.get());
    testLight = Light(1, 1, glm::vec3(4.5f, 4.5f, 0.5f), glm::vec3(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f);
}

Vox::Renderer::~Renderer()
{
}

void Vox::Renderer::Render(Editor* editor)
{
    // @TODO fix whatever is happening with vsync here
    UpdateViewportDimensions(editor);
    camera->SetAspectRatio(viewportTexture->GetHeight() == 0 ? 1 : viewportTexture->GetWidth() / viewportTexture->GetHeight());
    glViewport(0, 0, viewportTexture->GetWidth(), viewportTexture->GetHeight());

    RenderGBuffer();
    RenderDeferred();
    RenderSky();
    RenderDebugShapes();

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

//void Vox::Renderer::LoadTestModel(std::string path)
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

void Vox::Renderer::SetDebugPhysicsServer(std::shared_ptr<PhysicsServer> physicsServer)
{
    debugPhysicsServer = physicsServer;
}

void Vox::Renderer::SetCameraPosition(glm::vec3 position)
{
    camera->SetPosition(position);
}

void Vox::Renderer::SetCameraRotation(glm::vec3 rotation)
{
    camera->SetRotation(rotation);
}

void Vox::Renderer::SetCameraTarget(glm::vec3 target)
{
    camera->SetTarget(target);
}

Vox::Camera* Vox::Renderer::GetCurrentCamera() const
{
    return camera.get();
}

std::string Vox::Renderer::GetGlDebugTypeString(unsigned int errorCode)
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

void Vox::Renderer::UpdateViewportDimensions(Editor* editor)
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

void Vox::Renderer::RenderGBuffer()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->GetFramebufferId());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearDepth(0.0f);

    gBufferShader.Enable();
    {
        // Set camera matrices
        gBufferShader.SetUniformMatrix(gBufferViewMatrixLocation, camera->GetViewMatrix());
        gBufferShader.SetUniformMatrix(gBufferProjectionMatrixLocation, camera->GetProjectionMatrix());

        //if (IsModelValid(testModel))
        //{
        //    for (int i = 0; i < testModel.meshCount; ++i)
        //    {
        //        DrawMeshGBuffer(&testModel.meshes[i], &testModel.materials[testModel.meshMaterial[i]], testModel.transform);
        //    }
        //}
    }

    RenderVoxelGrid(testVoxelGrid.get());
}

//void Vox::Renderer::DrawMeshGBuffer(Mesh* mesh, Material* material, const Matrix& transform)
//{
//    const Color albedo = material->maps[MATERIAL_MAP_DIFFUSE].color;
//    Vector3 materialAlbedo = Vector3(
//        albedo.r / 255.0f,
//        albedo.g / 255.0f,
//        albedo.b / 255.0f
//    );
//    glUniform3fv(materialColorLocation, 1, reinterpret_cast<float*>(&materialAlbedo));
//    // The shader uniform location is automatically assigned when the shader is loaded by raylib
//    // SetShaderValueMatrix(gBufferShader, gBufferShader.locs[SHADER_LOC_MATRIX_MODEL], transform);
//    rlSetUniformMatrix(gBufferShader.locs[SHADER_LOC_MATRIX_MODEL], transform);
//
//    glBindVertexArray(mesh->vaoId);
//    glDrawElements(GL_TRIANGLES, mesh->triangleCount * 3, GL_UNSIGNED_SHORT, 0);
//}

void Vox::Renderer::RenderDeferred()
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
    deferredShader->SetCameraPosition(camera->GetPosition());
    glBindVertexArray(quad->GetVaoId());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFramebufferId());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Vox::Renderer::RenderVoxelGrid(VoxelGrid* voxelGrid)
{
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->GetFramebufferId());
    glClear(GL_DEPTH_BUFFER_BIT);
    voxelShader->Enable();
    voxelShader->SetModelMatrix(glm::translate(glm::mat4x4(1.0f), glm::vec3(voxelGrid->x, voxelGrid->y, voxelGrid->z)) /** MatrixScale(2.0f, 2.0f, 2.0f)*/);
    voxelShader->SetViewMatrix(camera->GetViewMatrix());
    voxelShader->SetProjectionMatrix(camera->GetProjectionMatrix());
    glBindVertexArray(voxelGrid->GetVaoId());
    voxelShader->SetArrayTexture(voxelTextures.get());
    glDrawElements(GL_TRIANGLES, voxelGrid->GetVertexCount(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(voxelMeshVao);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 1024);
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

    glEnable(GL_DEPTH_TEST);
    debugLineShader.Enable();
    physicsServer->GetDebugRenderer()->BindAndBufferLines();
    debugLineShader.SetUniformMatrix(debugLineMatrixLocation, camera->GetViewProjectionMatrix());
    glDrawArrays(GL_LINES, 0, physicsServer->GetDebugRenderer()->GetLineVertexCount());

    debugTriangleShader.Enable();
    physicsServer->GetDebugRenderer()->BindAndBufferTriangles();
    debugTriangleShader.SetUniformMatrix(debugTriangleMatrixLocation, camera->GetViewProjectionMatrix());
    glDrawArrays(GL_TRIANGLES, 0, physicsServer->GetDebugRenderer()->GetTriangleVertexCount());
}

void Vox::Renderer::RenderSky()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFramebuffer->GetFramebufferId());

    skyShader.Enable();
    glm::mat4x4 cameraRotation = camera->GetRotationMatrix();;
    glm::mat4x4 projection = camera->GetProjectionMatrix();
    glm::mat4x4 matrix = glm::inverse(projection * cameraRotation);
    skyShader.SetUniformMatrix(skyShader.GetUniformLocation("cameraWorldSpace"), matrix);
    // deferredFramebuffer->ActivateTextures();
    glBindVertexArray(quad->GetVaoId());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Vox::Renderer::CopyViewportToTexture(RenderTexture& texture)
{
    glBlitNamedFramebuffer(0, texture.GetFramebufferId(),
        0, 0, texture.GetWidth(), texture.GetHeight(),
        0, 0, texture.GetWidth(), texture.GetHeight(),
        GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, 0);
}
