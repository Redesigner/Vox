#pragma once

#include <memory>
#include "Vox.h"
#include "core/datatypes/ObjectContainer.h"
#include "core/datatypes/Ref.h"
#include "rendering/mesh/MeshInstance.h"
#include "rendering/mesh/Model.h"

namespace Vox
{
    class SceneRenderer;
    class MeshShader;
    class Model;
	class PickShader;
	class Shader;

	class MeshInstanceContainer
	{
	public:
		MeshInstanceContainer(SceneRenderer* owner, size_t size, const std::shared_ptr<Model>& mesh);

		void Render(const MaterialShader* shader);

	    void RenderInstance(const MeshShader* shader, const MeshInstance& meshInstance) const;

	    void RenderInstance(const MaterialShader* shader, const MeshInstance& meshInstance) const;

#ifdef EDITOR
		void Render(const PickShader* shader);

	    void RenderInstance(const PickShader* shader, const MeshInstance& meshInstance) const;
#endif

	    [[nodiscard]] SceneRenderer* GetOwner() const;

		Ref<MeshInstance> CreateMeshInstance();

		[[nodiscard]] size_t GetInstanceCount() const;

	private:
	    SceneRenderer* owner;
		std::shared_ptr<Model> mesh;
		ObjectContainer<MeshInstance> meshInstances;
	};
}
