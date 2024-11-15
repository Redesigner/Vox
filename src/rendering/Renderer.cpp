#include "Renderer.h"

#include "raylib.h"
#include "rlgl.h"

#include "editor/Editor.h"
#include "rendering/GBuffer.h"

Renderer::Renderer()
{
    // This is realtive to the build location -- I'll have to fix this later
    ChangeDirectory("../../../");

    TraceLog(LOG_INFO, GetWorkingDirectory());
    gBufferShader = LoadShader("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
    materialColorLocation = GetShaderLocation(gBufferShader, "materialAlbedo");
    gBufferShader.locs[SHADER_LOC_COLOR_DIFFUSE] = materialColorLocation;

    deferredShader = LoadShader("assets/shaders/deferred.vert", "assets/shaders/deferred.frag");
    deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition"); // set this for camera?


    gBuffer = std::make_unique<GBuffer>(800, 450);

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

    camera = { 0 };
    camera.position = Vector3(6.0f, 6.0f, 6.0f);    // Camera position
    camera.target = Vector3(0.0f, 2.0f, 0.0f);      // Camera looking at point
    camera.up = Vector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                            // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    lightUniformLocations = LightUniformLocations(deferredShader);
    testLight = Light(1, 1, Vector3(-2.0f, 2.0f, -2.0f), Vector3(), Vector4(255.0f, 255.0f, 255.0f, 255.0f));

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
}

void Renderer::Render(Editor* editor)
{
    // UpdateViewportDimensions(editor);

    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginDrawing();
    {
        RenderGBuffer();

        ClearBackground(BLACK);
        RenderDeferred();
        // editor->Draw(&viewportTexture);
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
    rlDisableFramebuffer();
    rlClearScreenBuffers();
}

void Renderer::RenderDeferred()
{
    BeginMode3D(camera);
    {
        rlDisableColorBlend();
        rlEnableShader(deferredShader.id);
        {
            testLight.UpdateLightValues(deferredShader, lightUniformLocations);
            gBuffer->ActivateTextures();
            rlLoadDrawQuad();
            rlDisableShader();
        }
        rlEnableColorBlend();
        EndMode3D();
    }

    gBuffer->CopyToFramebuffer();
    rlDisableFramebuffer();
}
