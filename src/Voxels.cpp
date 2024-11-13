#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"

#include "editor/Editor.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
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

    Camera camera = { 0 };
    camera.position = Vector3(6.0f, 6.0f, 6.0f);    // Camera position
    camera.target = Vector3(0.0f, 2.0f, 0.0f);      // Camera looking at point
    camera.up = Vector3(0.0f, 1.0f, 0.0f);          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
        ClearBackground(BLACK);

        editor->Draw();

        BeginMode3D(camera);
        if (IsModelValid(testModel))
        {
            DrawModel(testModel, Vector3(0.0f, 0.0f, 0.0f), 1.0f, WHITE);
        }
        EndMode3D();

        EndDrawing();
    }

    if (IsModelValid(testModel))
    {
        UnloadModel(testModel);
    }
    CloseWindow();

    return 0;
}
