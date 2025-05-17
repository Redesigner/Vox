#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Vox.h"
#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "rendering/Light.h"
#include "rendering/mesh/MeshInstanceContainer.h"
#include "rendering/mesh/VoxelMesh.h"
#include "rendering/shaders/ComputeShader.h"
#include "rendering/shaders/pixel_shaders/PixelShader.h"
#include "rendering/skeletal_mesh/SkeletalModel.h"
#include "shaders/pixel_shaders/mesh_shaders/MaterialShader.h"
#include "skeletal_mesh/SkeletalMeshInstanceContainer.h"

struct SDL_Window;
namespace Vox
{
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
    class StencilBuffer;
    class StencilShader;
    class UVec2Buffer;

	class Renderer
	{
	public:
		explicit Renderer(SDL_Window* window);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;

		Renderer& operator =(const Renderer& other) = delete;
		Renderer& operator =(Renderer&& other) = delete;

		void Render(Editor* editor);

		// void LoadTestModel(std::string path);

		Ref<Camera> CreateCamera();

		[[nodiscard]] Ref<Camera> GetCurrentCamera() const;

		void SetCurrentCamera(const Ref<Camera>& camera);

		/**
		 * @brief Uploads a glTF model to the GPU
		 * @param alias the name to identify the model by
		 * @param relativeFilePath file path of the model, relative to 'assets'
		 * @return true if the model was loaded successfully, false otherwise
		 */
		bool UploadModel(std::string alias, const std::string& relativeFilePath);

		bool UploadSkeletalModel(std::string alias, const std::string& relativeFilePath);

#ifdef EDITOR
	    void AddMeshOutline(const Ref<MeshInstance>& mesh);
	    void AddMeshOutline(const Ref<SkeletalMeshInstance>& mesh);

	    void ClearMeshOutlines();
#endif

		Ref<MeshInstance> CreateMeshInstance(const std::string& meshName);

	    Ref<SkeletalMeshInstance> CreateSkeletalMeshInstance(const std::string& meshName);

		static std::string GetGlDebugTypeString(unsigned int errorCode);

		DynamicRef<VoxelMesh> CreateVoxelMesh(glm::ivec2 position);

		[[nodiscard]] const std::unordered_map<std::string, MeshInstanceContainer>& GetMeshes() const;

		[[nodiscard]] const std::unordered_map<std::string, SkeletalMeshInstanceContainer>& GetSkeletalMeshes() const;

#ifdef EDITOR
		[[nodiscard]] PickContainer* GetPickContainer() const;
		[[nodiscard]] PickBuffer* GetPickBuffer() const;

	    void RegisterOverlayMesh(const Ref<MeshInstance>& meshInstance);

	    void ClearOverlays();
#endif

	private:
		void CheckViewportDimensions(const Editor* editor);

        void ResizeBuffers(int width, int height);

		void RenderGBuffer();

		void RenderDeferred();

#ifdef EDITOR
		void RenderPickBuffer();

	    void RenderOutline() const;

	    void RenderOverlay() const;
#endif

		void UpdateVoxelMeshes();

		void RenderVoxelMeshes();

		void RenderVoxelMesh(VoxelMesh& voxelMesh) const;

		void RenderSky();

		void RenderDebugShapes();

		static void CopyViewportToTexture(const RenderTexture& texture);

		void CreateMeshVao();

		void CreateSkeletalMeshVao();

		void CreateVoxelVao();

		std::unique_ptr<GBuffer> gBuffer;
		std::unique_ptr<ColorDepthFramebuffer> deferredFramebuffer;
		std::unique_ptr<DeferredShader> deferredShader;
		std::unique_ptr<MaterialShader> gBufferShader, gBufferShaderSkeleton;

#ifdef EDITOR
        std::unique_ptr<MaterialShader> overlayShader;

		std::unique_ptr<PickBuffer> pickBuffer;
		std::unique_ptr<PickShader> pickShader, pickShaderSkeleton;
		std::unique_ptr<PickContainer> pickContainer;

        std::unique_ptr<StencilBuffer> stencilBuffer;
	    std::unique_ptr<StencilShader> stencilShader, stencilShaderSkeleton;

	    std::unique_ptr<UVec2Buffer> outlineBuffer, outlineBuffer2;
	    std::unique_ptr<OutlineShader> outlineShader;
	    std::unique_ptr<OutlineShaderJump> outlineShaderJump;
	    std::unique_ptr<OutlineShaderDistance> outlineShaderDistance;
#endif

		std::unique_ptr<PixelShader> skyShader, debugLineShader, debugTriangleShader;
		int debugLineMatrixLocation, debugTriangleMatrixLocation = -1;

		std::unique_ptr<ArrayTexture> voxelTextures;
		std::unique_ptr<VoxelShader> voxelShader;
		DynamicObjectContainer<VoxelMesh> voxelMeshes;
		ComputeShader voxelGenerationShader;

		unsigned int meshVao = 0, voxelMeshVao = 0, skeletalMeshVao = 0;

		// A container holding all of our meshes to be retrieved later
		std::unordered_map<std::string, MeshInstanceContainer> uploadedModels;
		std::unordered_map<std::string, SkeletalMeshInstanceContainer> uploadedSkeletalModels;

#ifdef EDITOR
	    std::vector<Ref<MeshInstance>> outlinedMeshes;
	    std::vector<Ref<SkeletalMeshInstance>> outlinedSkeletalMeshes;
#endif

        std::vector<Ref<MeshInstance>> overlayMeshes;

		LightUniformLocations lightUniformLocations;
		Light testLight;

		ObjectContainer<Camera> cameras;
		Ref<Camera> currentCamera;

		SDL_Window* mainWindow;

		std::unique_ptr<FullscreenQuad> quad;

		std::unique_ptr<RenderTexture> viewportTexture;
	};
}