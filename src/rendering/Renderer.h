#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/vec3.hpp>

#include "core/datatypes/DynamicObjectContainer.h"
#include "core/datatypes/DynamicRef.h"
#include "rendering/Light.h"
#include "rendering/mesh/MeshInstanceContainer.h"
#include "rendering/mesh/Model.h"
#include "rendering/mesh/SkeletalModel.h"
#include "rendering/mesh/VoxelMesh.h"
#include "rendering/shaders/ComputeShader.h"
#include "rendering/shaders/PixelShader.h"

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
	class MeshInstance;
	class PhysicsServer;
	class RenderTexture;
	class VoxelShader;

	class Renderer
	{
	public:
		Renderer(SDL_Window* window);
		~Renderer();

		void Render(Editor* editor);

		// void LoadTestModel(std::string path);

		Ref<Camera> CreateCamera();

		Ref<Camera> GetCurrentCamera() const;

		void SetCurrentCamera(Ref<Camera> camera);

		/**
		 * @brief Uploads a glTF model to the GPU
		 * @param alias the name to identify the model by
		 * @param relativeFilePath file path of the model, relative to 'assets'
		 * @return true if the model was loaded successfully, false otherwise
		 */
		bool UploadModel(std::string alias, std::string relativeFilePath);

		bool UploadSkeletalModel(std::string alias, std::string relativeFilePath);

		Ref<MeshInstance> CreateMeshInstance(std::string meshName);

		static std::string GetGlDebugTypeString(unsigned int errorCode);

		DynamicRef<VoxelMesh> CreateVoxelMesh(glm::ivec2 position);

	private:
		void UpdateViewportDimensions(Editor* editor);

		void RenderGBuffer();

		// void DrawMeshGBuffer(Mesh* mesh, Material* material, const Matrix& transform);

		void RenderDeferred();

		void UpdateVoxelMeshes();

		void RenderVoxelMeshes();

		void RenderVoxelMesh(VoxelMesh& voxelMesh);

		void RenderSky();

		void RenderDebugShapes();

		void CopyViewportToTexture(RenderTexture& texture);

		void CreateMeshVao();

		void CreateVoxelVao();

		std::unique_ptr<GBuffer> gBuffer;
		std::unique_ptr<Framebuffer> deferredFramebuffer;
		std::unique_ptr<DeferredShader> deferredShader;
		PixelShader gBufferShader;
		int gBufferModelMatrixLocation, gBufferViewMatrixLocation, gBufferProjectionMatrixLocation;
		int gBufferAlbedoLocation, gBufferRoughnessLocation;
		unsigned int meshVao;

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