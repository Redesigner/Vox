//
// Created by steph on 5/20/2025.
//

#pragma once
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

#include "Light.h"
#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/Ref.h"
#include "mesh/MeshInstanceContainer.h"
#include "skeletal_mesh/SkeletalMeshInstanceContainer.h"

namespace Vox
{
    class VoxelMesh;
    class Renderer;
    class UVec2Buffer;
    class StencilBuffer;
    class PickBuffer;
    class RenderTexture;
    class ColorDepthFramebuffer;
    class GBuffer;
    class Camera;
}

namespace Vox
{
    /**
     * @brief A renderer for holding all the resources related to a 3D world
     * this contains all meshes and framebuffers
     */
    class WorldRenderer
    {
    public:
        WorldRenderer();

        void Draw();

    private:
        void DrawGBuffer();

        void DrawDeferredPass();

#ifdef EDITOR
        void DrawPickBuffer();

        void DrawOutline() const;

        void DrawOverlay() const;
#endif

        void DrawVoxels();

        void DrawSky() const;

        void DrawDebugShapes() const;

        void UpdateVoxels();

        void GenerateBuffers();

        void ConditionalResizeFramebuffers();

        Renderer* GetRenderer() const;

        //std::weak_ptr<Camera> currentCamera;
        Ref<Camera> currentCamera;

        glm::uvec2 viewportSize;

        // BUFFERS
        std::unique_ptr<GBuffer> gBuffer;
        std::unique_ptr<ColorDepthFramebuffer> deferredFramebuffer;

#ifdef EDITOR
        std::unique_ptr<PickBuffer> pickBuffer;
        std::unique_ptr<StencilBuffer> stencilBuffer;
        std::unique_ptr<UVec2Buffer> outlineBuffer, outlineBuffer2;
#endif

        // MESH INSTANCES
        std::unordered_map<std::string, MeshInstanceContainer> meshInstances;
        std::unordered_map<std::string, SkeletalMeshInstanceContainer> skeletalMeshInstances;
        DynamicObjectContainer<VoxelMesh> voxelMeshes;

        Light testLight;
        LightUniformLocations lightUniformLocations;

#ifdef EDITOR
        std::vector<Ref<MeshInstance>> outlinedMeshes;
        std::vector<Ref<SkeletalMeshInstance>> outlinedSkeletalMeshes;
        std::vector<Ref<MeshInstance>> overlayMeshes;
#endif
    };
} // Vox
