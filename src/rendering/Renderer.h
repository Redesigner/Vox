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
#include "shaders/pixel_shaders/mesh_shaders/GBufferShader.h"

namespace Vox
{
	class PickContainer;
}

namespace Vox
{
	class PickShader;
	class PickBuffer;
}

struct SDL_Window;

namespace Vox
{
	class ArrayTexture;
	class Camera;
	class DebugRenderer;
	class DeferredShader;
	class Editor;
	class SimpleFramebuffer;
	class FullscreenQuad;
	class GBuffer;
	class PhysicsServer;
	class RenderTexture;
	class VoxelShader;

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

		Ref<MeshInstance> CreateMeshInstance(const std::string& meshName);

		static std::string GetGlDebugTypeString(unsigned int errorCode);

		DynamicRef<VoxelMesh> CreateVoxelMesh(glm::ivec2 position);

		[[nodiscard]] const std::unordered_map<std::string, MeshInstanceContainer>& GetMeshes() const;

		[[nodiscard]] const std::unordered_map<std::string, SkeletalModel>& GetSkeletalMeshes() const;

#ifdef EDITOR
		[[nodiscard]] PickContainer* GetPickContainer() const;
		[[nodiscard]] PickBuffer* GetPickBuffer() const;
#endif

	private:
		void UpdateViewportDimensions(const Editor* editor);

		void RenderGBuffer();

		void RenderDeferred();

#ifdef EDITOR
		void RenderPickBuffer();
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
		std::unique_ptr<SimpleFramebuffer> deferredFramebuffer;
		
		std::unique_ptr<DeferredShader> deferredShader;
		std::unique_ptr<GBufferShader> gBufferShader;

#ifdef EDITOR
		std::unique_ptr<PickBuffer> pickBuffer;
		std::unique_ptr<PickShader> pickShader;
		std::unique_ptr<PickContainer> pickContainer;
#endif
		
		unsigned int meshVao;

		std::unique_ptr<PixelShader> skeletalMeshShader, skyShader, debugLineShader, debugTriangleShader;
		int skeletalModelMatrixLocation, skeletalViewMatrixLocation, skeletalProjectionMatrixLocation;
		int skeletalAlbedoLocation, skeletalRoughnessLocation;
		int debugLineMatrixLocation, debugTriangleMatrixLocation = 0;
		unsigned int skeletalMeshVao;

		std::unique_ptr<ArrayTexture> voxelTextures;
		std::unique_ptr<VoxelShader> voxelShader;
		DynamicObjectContainer<VoxelMesh> voxelMeshes;
		ComputeShader voxelGenerationShader;
		unsigned int voxelMeshVao;

		// A container holding all of our meshes to be retrieved later
		std::unordered_map<std::string, MeshInstanceContainer> uploadedModels;

		std::unordered_map<std::string, SkeletalModel> uploadedSkeletalModels;


		LightUniformLocations lightUniformLocations;
		Light testLight;

		Ref<Camera> currentCamera;

		ObjectContainer<Camera> cameras;

		SDL_Window* mainWindow;

		std::unique_ptr<FullscreenQuad> quad;

		std::unique_ptr<RenderTexture> viewportTexture;
	};
}