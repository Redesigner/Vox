#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Vox.h"
#include "core/datatypes/DynamicObjectContainer.h"
#include "rendering/Light.h"
#include "rendering/mesh/MeshInstanceContainer.h"
#include "rendering/mesh/VoxelMesh.h"
#include "rendering/shaders/pixel_shaders/PixelShader.h"
#include "rendering/skeletal_mesh/SkeletalModel.h"
#include "shaders/pixel_shaders/mesh_shaders/MaterialShader.h"
#include "voxel/VoxelMaterial.h"

struct SDL_Window;

namespace Vox
{
    class SkyShader;
    class DebugShader;
    class ArrayTexture;
	class Camera;
	class ColorDepthFramebuffer;
	class DebugRenderer;
	class DeferredShader;
	class Editor;
	class FullscreenQuad;
	class GBuffer;
	class PhysicsServer;
	class PickBuffer;
	class PickContainer;
	class PickShader;
	class RenderTexture;
	class VoxelShader;
    class OutlineShader;
    class OutlineShaderDistance;
    class OutlineShaderJump;
    class SceneRenderer;
    class StencilBuffer;
    class StencilShader;
    class UVec2Buffer;
    class VoxelGenerationShader;

	class Renderer
	{
	public:
		explicit Renderer(SDL_Window* window);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;

		Renderer& operator =(const Renderer& other) = delete;
		Renderer& operator =(Renderer&& other) = delete;

	    [[nodiscard]] SDL_Window* GetWindow() const;

#ifdef EDITOR
		void Render(Editor* editor);

	    void RegisterScene(const std::shared_ptr<SceneRenderer>& scene);
#elif
	    void Render();
#endif
		/**
		 * @brief Uploads a glTF model to the GPU
		 * @param alias name to identify the model by
		 * @param relativeFilePath file path of the model, relative to 'assets'
		 * @return true if the model was loaded successfully, false otherwise
		 */
		bool UploadModel(std::string alias, const std::string& relativeFilePath);

        /**
         * @brief Uploads a skeletal model (glTF) to the GPU
         * @param alias name key to be used
         * @param relativeFilePath file path of the model, relative to 'assets' folder
         * @return true if the model was loaded successfully, false otherwise
         */
        bool UploadSkeletalModel(std::string alias, const std::string& relativeFilePath);

        /**
         * @brief Convert GL error code into human-readable string
         * @param errorCode error code returned by OpenGL
         * @return human-readable string
         */
        static std::string GetGlDebugTypeString(unsigned int errorCode);

		[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<Model>>& GetMeshes() const;

		[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<SkeletalModel>>& GetSkeletalMeshes() const;

        // INSTANCES
	    [[nodiscard]] std::shared_ptr<Model> GetMesh(const std::string& name) const;
	    [[nodiscard]] std::shared_ptr<SkeletalModel> GetSkeletalMesh(const std::string& name) const;
	    [[nodiscard]] const VoxelMaterial* GetVoxelMaterial(unsigned int materialIndex) const;

	    // SCENE RENDERER NECESSARY METHODS
	    [[nodiscard]] MaterialShader* GetGBufferShader() const;
	    void BindMeshVao() const;

	    [[nodiscard]] MaterialShader* GetGBufferShaderSkeleton() const;
	    void BindSkeletalMeshVao() const;

	    [[nodiscard]] DeferredShader* GetDeferredShader() const;
	    void BindQuadVao() const;

	    [[nodiscard]] SkyShader* GetSkyShader() const;

	    [[nodiscard]] VoxelShader* GetVoxelMeshShader() const;
	    [[nodiscard]] VoxelGenerationShader* GetVoxelGenerationShader() const;
	    [[nodiscard]] ArrayTexture* GetVoxelTextures() const;
	    void BindVoxelMeshVao() const;

	    [[nodiscard]] DebugShader* GetDebugLineShader() const;
	    [[nodiscard]] DebugShader* GetDebugTriangleShader() const;

#ifdef EDITOR
	    [[nodiscard]] PickShader* GetPickShader() const;
	    [[nodiscard]] PickShader* GetPickShaderSkeleton() const;

	    [[nodiscard]] StencilShader* GetStencilShader() const;
	    [[nodiscard]] StencilShader* GetStencilShaderSkeleton() const;

	    [[nodiscard]] OutlineShader* GetOutlineShader() const;
	    [[nodiscard]] OutlineShaderDistance* GetOutlineShaderDistance() const;
	    [[nodiscard]] OutlineShaderJump* GetOutlineShaderJump() const;

	    [[nodiscard]] MaterialShader* GetOverlayShader() const;
#endif

	private:
	    void LoadDefaultShaders();

		static void CopyViewportToTexture(const RenderTexture& texture);

		void CreateMeshVao();

		void CreateSkeletalMeshVao();

		void CreateVoxelVao();

	    // SHADERS
		std::unique_ptr<DeferredShader> deferredShader;
		std::unique_ptr<MaterialShader> gBufferShader, gBufferShaderSkeleton;

		std::unique_ptr<VoxelShader> voxelShader;
		std::unique_ptr<VoxelGenerationShader> voxelGenerationShader;

		std::unique_ptr<SkyShader> skyShader;
	    std::unique_ptr<DebugShader> debugLineShader, debugTriangleShader;
#ifdef EDITOR
		std::unique_ptr<PickShader> pickShader, pickShaderSkeleton;
		std::unique_ptr<PickContainer> pickContainer;

	    std::unique_ptr<StencilShader> stencilShader, stencilShaderSkeleton;

        std::unique_ptr<MaterialShader> overlayShader;
	    std::unique_ptr<OutlineShader> outlineShader;
	    std::unique_ptr<OutlineShaderJump> outlineShaderJump;
	    std::unique_ptr<OutlineShaderDistance> outlineShaderDistance;
#endif

		std::unique_ptr<ArrayTexture> voxelTextures;

		unsigned int meshVao = 0, voxelMeshVao = 0, skeletalMeshVao = 0;

		// INSTANCES
		std::unordered_map<std::string, std::shared_ptr<Model>> uploadedMeshes;
		std::unordered_map<std::string, std::shared_ptr<SkeletalModel>> uploadedSkeletalMeshes;
	    std::vector<VoxelMaterial> voxelMaterials;

		SDL_Window* mainWindow;

		std::unique_ptr<FullscreenQuad> quad;

	    // If the editor is active, we can render multiple scenes
#ifndef EDITOR
	    std::unique_ptr<SceneRenderer> sceneRenderer;
#else
        std::vector<std::weak_ptr<SceneRenderer>> activeScenes;
#endif
	};
}