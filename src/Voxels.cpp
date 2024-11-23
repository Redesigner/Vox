#include "Voxels.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"

#include "editor/Editor.h"
#include "rendering/Renderer.h"
#include "voxel/Octree.h"
#include "voxel/Voxel.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Voxels");
    {
        SetTargetFPS(60);
        rlImGuiSetup(true);

        std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
        std::unique_ptr<Editor> editor = std::make_unique<Editor>();

        editor->BindOnGLTFOpened([&renderer](std::string fileName)
            {
                renderer->LoadTestModel(fileName);
            });

        Voxel testVoxel;
        testVoxel.materialId = 1;

        std::unique_ptr<Octree::Node> octree = std::make_unique<Octree::Node>(16);
        octree->SetVoxel(0, 0, 0, &testVoxel);
        octree->SetVoxel(0, 0, 1, &testVoxel);
        octree->SetVoxel(0, 1, 0, &testVoxel);
        octree->SetVoxel(0, 1, 1, &testVoxel);
        octree->SetVoxel(1, 0, 0, &testVoxel);
        octree->SetVoxel(1, 0, 1, &testVoxel);
        octree->SetVoxel(1, 1, 0, &testVoxel);
        octree->SetVoxel(1, 1, 1, &testVoxel);

        testVoxel.materialId = 2;
        octree->SetVoxel(0, 0, 0, &testVoxel);

        testVoxel = *octree->GetVoxel(0, 0, 0);

        while (!WindowShouldClose())
        {
            renderer->Render(editor.get());
        }
    }
    CloseWindow();

    return 0;
}
