#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/vec3.hpp>

#include "core/datatypes/Ref.h"
#include "rendering/Light.h"
#include "rendering/shaders/ComputeShader.h"
#include "rendering/shaders/PixelShader.h"
#include "rendering/VoxelMesh.h"

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
		Renderer(SDL_Window* window);
		~Renderer();

		void Render(Editor* editor);

		// void LoadTestModel(std::string path);

		void SetDebugPhysicsServer(std::shared_ptr<PhysicsServer> physicsServer);

		void SetCameraPosition(glm::vec3 position);

		void SetCameraRotation(glm::vec3 rotation);

		void SetCameraTarget(glm::vec3 target);

		Camera* GetCurrentCamera() const;

		static std::string GetGlDebugTypeString(unsigned int errorCode);

		Ref<VoxelMesh> CreateVoxelMesh(glm::ivec2 position);

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

		void CreateVoxelVao();

		std::unique_ptr<GBuffer> gBuffer;
		std::unique_ptr<Framebuffer> deferredFramebuffer;
		std::unique_ptr<DeferredShader> deferredShader;
		PixelShader gBufferShader;
		int gBufferModelMatrixLocation, gBufferViewMatrixLocation, gBufferProjectionMatrixLocation;

		std::unique_ptr<ArrayTexture> voxelTextures;
		std::unique_ptr<VoxelShader> voxelShader;
		std::vector<VoxelMesh> voxelMeshes;
		ComputeShader voxelGenerationShader;
		unsigned int voxelMeshVao;

		PixelShader skyShader;

		std::weak_ptr<Vox::PhysicsServer> debugPhysicsServer;
		PixelShader debugLineShader, debugTriangleShader;
		int debugLineMatrixLocation, debugTriangleMatrixLocation = 0;

		LightUniformLocations lightUniformLocations;
		Light testLight;
		int materialColorLocation, materialRoughnessLocation = 0;

		std::unique_ptr<Vox::Camera> camera;

		SDL_Window* mainWindow;

		std::unique_ptr<FullscreenQuad> quad;

		std::unique_ptr<RenderTexture> viewportTexture;
		// Material defaultMaterial;
		// Model testModel;
	};
}