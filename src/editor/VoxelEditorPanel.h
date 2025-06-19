//
// Created by steph on 6/16/2025.
//

#pragma once
#include "core/datatypes/DelegateHandle.h"
#include "voxel/VoxelWorld.h"

namespace Vox
{

    class VoxelEditorPanel
    {
    public:
        explicit VoxelEditorPanel(VoxelWorld* world);

        ~VoxelEditorPanel();

        void Draw();

    private:
        VoxelWorld* world;

        int voxelMaterialId;

        DelegateHandle<int, int> raycastClickDelegate;
    };

} // Vox
