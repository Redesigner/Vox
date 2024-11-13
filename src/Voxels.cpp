#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"

#include "editor/Editor.h"
#include "rendering/GBuffer.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        SetTargetFPS(60);

        rlImGuiSetup(true);

        Model testModel;
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();
        editor->BindOnGLTFOpened([&testModel](std::string fileName)
            {
                if (IsModelValid(testModel))
                {
                    UnloadModel(testModel);
                }
                testModel = LoadModel(fileName.c_str());
            });

        // This is realtive to the build location -- I'll have to fix this later
        ChangeDirectory("../../../");

        TraceLog(LOG_INFO, GetWorkingDirectory());
        Shader gBufferShader = LoadShader("assets/shaders/gBuffer.vert", "assets/shaders/gBuffer.frag");
        Shader deferredShader = LoadShader("assets/shaders/deferred.vert", "assets/shaders/deferred.frag");
        deferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(deferredShader, "viewPosition"); // set this for camera?

        GBuffer* gBuffer = new GBuffer(800, 450);
        rlEnableShader(deferredShader.id);

        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gPosition"), 0);
        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gNormal"), 1);
        rlSetUniformSampler(rlGetLocationUniform(deferredShader.id, "gAlbedoSpec"), 2);

        rlDisableShader();

        Camera camera = { 0 };
        camera.position = Vector3(6.0f, 6.0f, 6.0f);    // Camera position
        camera.target = Vector3(0.0f, 2.0f, 0.0f);      // Camera looking at point
        camera.up = Vector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
        camera.fovy = 45.0f;                            // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;

        RenderTexture2D viewportTexture = RenderTexture2D();

        while (!WindowShouldClose())
        {
            UpdateCamera(&camera, CAMERA_ORBITAL);
            // if using editor
            {
                // Resize our render texture if it's the wrong size, so we get a 1:1 resolution for the editor viewport
                Vector2 editorViewportSize = editor->GetViewportDimensions();
                if (IsRenderTextureValid(viewportTexture))
                {
                    if (viewportTexture.texture.width != static_cast<int>(editorViewportSize.x) || viewportTexture.texture.height != static_cast<int>(editorViewportSize.y))
                    {
                        UnloadRenderTexture(viewportTexture);
                        viewportTexture = LoadRenderTexture(editorViewportSize.x, editorViewportSize.y);
                    }
                }
                else
                {
                    viewportTexture = LoadRenderTexture(editorViewportSize.x, editorViewportSize.y);
                }

                BeginTextureMode(viewportTexture);
                ClearBackground(DARKGRAY);
                BeginMode3D(camera);
                if (IsModelValid(testModel))
                {
                    DrawModel(testModel, Vector3(0.0f, 0.0f, 0.0f), 1.0f, WHITE);
                }
                EndMode3D();
                EndTextureMode();
            }

            BeginDrawing();
            ClearBackground(BLACK);


            editor->Draw(&viewportTexture);

            EndDrawing();
        }

        if (IsModelValid(testModel))
        {
            UnloadModel(testModel);
        }

        if (IsRenderTextureValid(viewportTexture))
        {
            UnloadRenderTexture(viewportTexture);
        }

        delete gBuffer;
    }
    CloseWindow();

    return 0;
}
