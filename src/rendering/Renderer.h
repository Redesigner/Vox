#pragma once

#include <memory>
#include <string>

#include <glm/vec3.hpp>

#include "rendering/Light.h"
#include "rendering/shaders/Shader.h"
#include "rendering/Texture.h"

class ArrayTexture;
class DebugRenderer;
class DeferredShader;
class Editor;
class Framebuffer;
class GBuffer;
class VoxelGrid;
class VoxelShader;

namespace Vox
{
	class Camera;
	class PhysicsServer;
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Render(Editor* editor);

		void LoadTestModel(std::string path);

		void SetDebugPhysicsServer(std::shared_ptr<PhysicsServer> physicsServer);

		void SetCameraPosition(glm::vec3 position);

		void SetCameraRotation(glm::vec3 rotation);

		void SetCameraTarget(glm::vec3 target);

		Camera* GetCurrentCamera() const;

	private:
		void UpdateViewportDimensions(Editor* editor);

		void RenderGBuffer();

		// void DrawMeshGBuffer(Mesh* mesh, Material* material, const Matrix& transform);

		void RenderDeferred();

		void RenderVoxelGrid(VoxelGrid* voxelGrid);

		void RenderSky();

		void RenderDebugShapes();

		void CopyViewportToTexture(Texture& texture);

		std::unique_ptr<GBuffer> gBuffer;
		std::unique_ptr<Framebuffer> deferredFramebuffer;

		std::shared_ptr<VoxelGrid> testVoxelGrid;
		std::unique_ptr<ArrayTexture> voxelTextures;

		std::weak_ptr<Vox::PhysicsServer> debugPhysicsServer;

		std::unique_ptr<VoxelShader> voxelShader;
		std::unique_ptr<DeferredShader> deferredShader;
		Shader gBufferShader;
		Shader skyShader;
		Shader debugLineShader, debugTriangleShader;

		int debugLineMatrixLocation, debugTriangleMatrixLocation = 0;

		LightUniformLocations lightUniformLocations;
		Light testLight;
		int materialColorLocation, materialRoughnessLocation = 0;

		std::unique_ptr<Vox::Camera> camera;

		Texture viewportTexture;
		// Material defaultMaterial;
		// Model testModel;
	};
}