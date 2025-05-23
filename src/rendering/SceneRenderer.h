//
// Created by steph on 5/20/2025.
//

#pragma once
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

#include "Light.h"
#include "rendering/Camera.h"
#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "core/datatypes/Ref.h"
#include "mesh/MeshInstanceContainer.h"
#include "skeletal_mesh/SkeletalMeshInstanceContainer.h"

namespace Vox
{
    class EditorViewport;
}

namespace Vox
{
    class Camera;
    class ColorDepthFramebuffer;
    class GBuffer;
    class PickBuffer;
    class PickContainer;
    class RenderTexture;
    class Renderer;
    class StencilBuffer;
    class UVec2Buffer;
    class VoxelMesh;

    /**
     * @brief A renderer for holding all the resources related to a 3D world
     * this contains all meshes and framebuffers
     */
    class SceneRenderer
    {
    public:
        SceneRenderer();
        ~SceneRenderer();

        void Draw();

        Ref<MeshInstance> CreateMeshInstance(const std::string& meshName);

        Ref<SkeletalMeshInstance> CreateSkeletalMeshInstance(const std::string& meshName);

        DynamicRef<VoxelMesh> CreateVoxelMesh(glm::ivec2 chunkLocation);

        [[nodiscard]] Ref<Camera> CreateCamera();

        [[nodiscard]] Ref<Camera> GetCurrentCamera() const;

        void SetCurrentCamera(const Ref<Camera>& camera);

        [[nodiscard]] ColorDepthFramebuffer* GetTexture() const;

        void SetSize(unsigned int x, unsigned int y);

#ifdef EDITOR
        [[nodiscard]] PickContainer* GetPickContainer() const;

        void AddMeshOutline(const Ref<MeshInstance>& mesh);

        void AddMeshOutline(const Ref<SkeletalMeshInstance>& mesh);

        void RegisterOverlayMesh(const Ref<MeshInstance>& mesh);

        void ClearMeshOutlines();

        void ClearOverlays();

        [[nodiscard]] PickBuffer* GetPickBuffer() const;

        std::weak_ptr<EditorViewport> viewport;
#endif

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

        [[nodiscard]] Renderer* GetRenderer() const;

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
        std::unique_ptr<PickContainer> pickContainer;
#endif

        // MESH INSTANCES
        std::unordered_map<std::string, MeshInstanceContainer> meshInstances;
        std::unordered_map<std::string, SkeletalMeshInstanceContainer> skeletalMeshInstances;
        DynamicObjectContainer<VoxelMesh> voxelMeshes;
        ObjectContainer<Camera> cameras;

        Light testLight;
        LightUniformLocations lightUniformLocations;

#ifdef EDITOR
        std::vector<Ref<MeshInstance>> outlinedMeshes;
        std::vector<Ref<SkeletalMeshInstance>> outlinedSkeletalMeshes;
        std::vector<Ref<MeshInstance>> overlayMeshes;
#endif
    };
} // Vox
