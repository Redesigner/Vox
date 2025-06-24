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
        explicit VoxelEditorPanel(const std::shared_ptr<World>& world);

        ~VoxelEditorPanel();

        void Draw();

    private:
        std::weak_ptr<World> world;

        int voxelMaterialId;

        DelegateHandle<int, int> raycastClickDelegate;
    };

} // Vox
