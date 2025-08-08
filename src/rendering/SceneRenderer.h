//
// Created by steph on 5/20/2025.
//

#pragma once
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>

#include "Light.h"
#include "camera/Camera.h"
#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "core/datatypes/Ref.h"
#include "core/datatypes/WeakRef.h"
#include "mesh/MeshInstanceContainer.h"
#include "skeletal_mesh/SkeletalMeshInstanceContainer.h"

namespace Vox
{
    class World;
    class FlyCamera;
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
        explicit SceneRenderer(World* world);
        ~SceneRenderer();

        void Draw();

        Ref<MeshInstance> CreateMeshInstance(const std::string& meshName);

        Ref<SkeletalMeshInstance> CreateSkeletalMeshInstance(const std::string& meshName);

        DynamicRef<VoxelMesh> CreateVoxelMesh(glm::ivec2 chunkLocation);

        [[nodiscard]] std::shared_ptr<Camera> GetCurrentCamera() const;

        void SetCurrentCamera(const std::shared_ptr<Camera>& camera);

        void ResetCamera();

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

        bool renderDebug = false;

    private:
        void DrawGBuffer();

        void DrawDeferredPass();

#ifdef EDITOR
        void DrawPickBuffer();

        void DrawOutline();

        void DrawOverlay();
#endif

        void DrawVoxels();

        void DrawSky() const;

        void DrawDebugShapes() const;

        void UpdateVoxels();

        void GenerateBuffers();

        void ConditionalResizeFramebuffers();

        [[nodiscard]] static Renderer* GetRenderer();

        std::shared_ptr<Camera> currentCamera;
        std::shared_ptr<FlyCamera> defaultCamera;

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

        Light testLight;

        World* owningWorld;

#ifdef EDITOR
        std::vector<WeakRef<MeshInstance>> outlinedMeshes;
        std::vector<WeakRef<SkeletalMeshInstance>> outlinedSkeletalMeshes;
        std::vector<WeakRef<MeshInstance>> overlayMeshes;
#endif
    };
} // Vox
