#pragma once

#include "Renderer.h"

#include "raylib.h"
#include "rlgl.h"

#include "editor/Editor.h"
#include "rendering/GBuffer.h"
#include "rendering/Framebuffer.h"

Renderer::Renderer()
{
    // This is realtive to the build location -- I'll have to fix this later
    ChangeDirectory("../../../");

    TraceLog(LOG_INFO, TextFormat("Set current working directory to '%s'", GetWorkingDirectory()));
    gBufferShader = LoadShader("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
    materialColorLocation = GetShaderLocation(gBufferShader, "materialAlbedo");

    deferredShader = LoadShader("assets/shaders/deferred.vert", "assets/shaders/deferred.frag");
    deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition"); // set this for camera?

    skyShader = LoadShader("assets/shaders/sky.vert", "assets/shaders/sky.frag");

    gBuffer = std::make_unique<GBuffer>(800, 450);
    deferredFramebuffer = std::make_unique<Framebuffer>(800, 450);

    rlEnableShader(deferredShader.id);
    {
        int position = 0;
        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gPosition"), 0);
        rlSetUniform(GetShaderLocation(deferredShader, "gPosition"), &position, SHADER_UNIFORM_INT, 1);

        position = 1;
        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gNormal"), 1);
        rlSetUniform(GetShaderLocation(deferredShader, "gNormal"), &position, SHADER_UNIFORM_INT, 1);

        position = 2;
        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gAlbedoSpec"), 2);
        rlSetUniform(GetShaderLocation(deferredShader, "gAlbedoSpec"), &position, SHADER_UNIFORM_INT, 1);
    }
    rlDisableShader();

    rlEnableShader(skyShader.id);
    {
        int position = 0;
        rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "color"), position);
        // rlSetUniform(GetShaderLocation(deferredShader, "color"), &position, SHADER_UNIFORM_INT, 1);

        position = 1;
        rlSetUniformSampler(rlGetLocationUniform(skyShader.id, "depth"), position);
        // rlSetUniform(GetShaderLocation(deferredShader, "depth"), &position, SHADER_UNIFORM_INT, 1);
        rlDisableShader();
    }

    camera = { 0 };
    camera.position = Vector3(6.0f, 6.0f, 6.0f);    // Camera position
    camera.target = Vector3(0.0f, 2.0f, 0.0f);      // Camera looking at point
    camera.up = Vector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                            // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    lightUniformLocations = LightUniformLocations(deferredShader);
    testLight = Light(1, 1, Vector3(-2.0f, 2.0f, -2.0f), Vector3(), Vector4(255.0f, 255.0f, 255.0f, 255.0f), 1000.0f);

    viewportTexture = RenderTexture2D();
    testModel = LoadModel("assets/models/mushroom.glb");
    for (int i = 0; i < testModel.materialCount; ++i)
    {
        testModel.materials[i].shader = gBufferShader;
    }
}

Renderer::~Renderer()
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
    if (IsShaderValid(deferredShader))
    {
        UnloadShader(deferredShader);
    }
    if (IsShaderValid(skyShader))
    {
        UnloadShader(skyShader);
    }
}

void Renderer::Render(Editor* editor)
{
    UpdateViewportDimensions(editor);

    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginDrawing();
    {
        RenderGBuffer();

        RenderDeferred();
        // RenderSky();
        deferredFramebuffer->BindRead();

        // Copy into texture to be rendered with imgui
        rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, viewportTexture.id);
        rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00004000);
        rlDisableFramebuffer();

        // Make sure that our viewport size matches the window size when drawing with imgui
        rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
        editor->Draw(&viewportTexture);
        EndDrawing();
    }
}

void Renderer::LoadTestModel(std::string path)
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

void Renderer::UpdateViewportDimensions(Editor* editor)
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
        }
    }
    else
    {
        viewportTexture = LoadRenderTexture(editorViewportSize.x, editorViewportSize.y);
    }
}

void Renderer::RenderGBuffer()
{
    ClearBackground(BLACK);
    gBuffer->EnableFramebuffer();
    rlClearScreenBuffers();

    rlDisableColorBlend();

    rlEnableShader(gBufferShader.id);
    {
        BeginMode3D(camera);
        {
            if (IsModelValid(testModel))
            {
                for (int i = 0; i < testModel.meshCount; ++i)
                {
                    const Color albedo = testModel.materials[testModel.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;
                    Vector3 materialAlbedo = Vector3(
                        albedo.r / 255.0f,
                        albedo.g / 255.0f,
                        albedo.b / 255.0f
                    );
                    rlEnableShader(gBufferShader.id);
                    rlSetUniform(materialColorLocation, &materialAlbedo, SHADER_UNIFORM_VEC3, 1);
                    rlDisableShader();
                    DrawMesh(testModel.meshes[i], testModel.materials[i], testModel.transform);
                }
            }
            EndMode3D();
        }
        rlDisableShader();
    }
    rlEnableColorBlend();
}

void Renderer::RenderDeferred()
{
    // rlViewport(0, 0, viewportTexture.texture.width, viewportTexture.texture.height);
    
    rlSetFramebufferWidth(viewportTexture.texture.width);
    rlSetFramebufferHeight(viewportTexture.texture.height);

    gBuffer->BindRead();
    deferredFramebuffer->BindDraw();

    rlDisableColorBlend();
    rlDisableDepthMask();
    rlEnableShader(deferredShader.id);
    {
        testLight.UpdateLightValues(deferredShader, lightUniformLocations);
        gBuffer->ActivateTextures();
        deferredFramebuffer->ActivateTextures();
        rlLoadDrawQuad();
        rlDisableShader();
    }
    rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00000100);
    rlEnableColorBlend();
    rlEnableDepthMask();
}

void Renderer::RenderSky()
{
    rlDisableDepthMask();
    rlEnableShader(skyShader.id);
    {
        deferredFramebuffer->BindRead();
        rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, 0);
        rlLoadDrawQuad();
        rlDisableFramebuffer();
        rlDisableShader();
    }
    rlEnableDepthMask();
}

void Renderer::CopyViewportToTexture(RenderTexture2D& texture)
{
    rlBindFramebuffer(RL_READ_FRAMEBUFFER, 0);
    rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, viewportTexture.id);
    rlBlitFramebuffer(0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0, 0, viewportTexture.texture.width, viewportTexture.texture.height, 0x00004000 | 0x00000100);
    rlDisableFramebuffer();
}
