#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "rendering/Light.h"
#include "rendering/mesh/MeshInstanceContainer.h"
#include "rendering/skeletalmesh/SkeletalModel.h"
#include "rendering/mesh/VoxelMesh.h"
#include "rendering/shaders/ComputeShader.h"
#include "rendering/shaders/pixel_shaders/PixelShader.h"
#include "shaders/pixel_shaders/GBufferShader.h"

struct SDL_Window;

namespace Vox
{
	class ArrayTexture;
	class Camera;
	class DebugRenderer;
	class DeferredShader;
	class Editor;
	class Framebuffer;
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

		Ref<Camera> GetCurrentCamera() const;

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

	private:
		void UpdateViewportDimensions(const Editor* editor);

		void RenderGBuffer();

		void RenderDeferred();

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
		std::unique_ptr<Framebuffer> deferredFramebuffer;
		std::unique_ptr<DeferredShader> deferredShader;
		std::unique_ptr<GBufferShader> gBufferShader;

		unsigned int meshVao;

		PixelShader skeletalMeshShader;
		int skeletalModelMatrixLocation, skeletalViewMatrixLocation, skeletalProjectionMatrixLocation;
		int skeletalAlbedoLocation, skeletalRoughnessLocation;
		unsigned int skeletalMeshVao;

		std::unique_ptr<ArrayTexture> voxelTextures;
		std::unique_ptr<VoxelShader> voxelShader;
		DynamicObjectContainer<VoxelMesh> voxelMeshes;
		ComputeShader voxelGenerationShader;
		unsigned int voxelMeshVao;

		// A container holding all of our meshes to be retrieved later
		std::unordered_map<std::string, MeshInstanceContainer> uploadedModels;

		std::unordered_map<std::string, SkeletalModel> uploadedSkeletalModels;

		PixelShader skyShader;
		PixelShader debugLineShader, debugTriangleShader;
		int debugLineMatrixLocation, debugTriangleMatrixLocation = 0;

		LightUniformLocations lightUniformLocations;
		Light testLight;

		Ref<Camera> currentCamera;

		ObjectContainer<Camera> cameras;

		SDL_Window* mainWindow;

		std::unique_ptr<FullscreenQuad> quad;

		std::unique_ptr<RenderTexture> viewportTexture;
	};
}