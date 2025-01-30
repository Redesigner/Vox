#include "Model.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <tiny_gltf.h>

#include "core/logging/Logging.h"
#include "rendering/shaders/Shader.h"

namespace Vox
{
	Model::Model(std::string filepath)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		tinygltf::Model model;
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);

		// Buffer our data into opengl
		// One GL buffer corresponds to a bufferview
		bufferIds = std::vector<unsigned int>(model.bufferViews.size(), 0);
		glCreateBuffers(model.bufferViews.size(), bufferIds.data());
		for (int i = 0; i < model.bufferViews.size(); ++i)
		{
			tinygltf::BufferView bufferView = model.bufferViews[i];
			glNamedBufferData(bufferIds[i], bufferView.byteLength, model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, GL_STATIC_DRAW);
		}

		for (const tinygltf::Material& material : model.materials)
		{
			const std::vector<double>& color = material.pbrMetallicRoughness.baseColorFactor;
			materials.emplace_back(glm::vec4(color[0], color[1], color[2], color[3]), material.pbrMetallicRoughness.roughnessFactor, material.pbrMetallicRoughness.metallicFactor);
		}

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				if (primitive.attributes.size() > 3)
				{
					VoxLog(Warning, Rendering, "GLTF model has more than three attributes. Models currently only support having POSITION, NORMAL, and TEXCOORD_0.");
				}

				auto positionBuffer = primitive.attributes.find("POSITION");
				if (positionBuffer == primitive.attributes.end())
				{
					VoxLog(Error, Rendering, "GLTF model does not have a position attribute.");
					break;
				}

				auto normalBuffer = primitive.attributes.find("NORMAL");
				if (normalBuffer == primitive.attributes.end())
				{
					VoxLog(Error, Rendering, "GLTF model does not have a normal attribute.");
					break;
				}

				auto uvBuffer = primitive.attributes.find("TEXCOORD_0");
				if (uvBuffer == primitive.attributes.end())
				{
					VoxLog(Error, Rendering, "GLTF model does not have a texcoord attribute.");
					break;
				}

				const unsigned int indexCount = model.accessors[primitive.indices].count;
				const unsigned int indexBufferId = bufferIds[model.accessors[primitive.indices].bufferView];
				const unsigned int positionBufferId = bufferIds[model.accessors[positionBuffer->second].bufferView];
				const unsigned int normalBufferId = bufferIds[model.accessors[normalBuffer->second].bufferView];
				const unsigned int uvBufferId = bufferIds[model.accessors[uvBuffer->second].bufferView];
				meshes.emplace_back(indexCount, model.accessors[primitive.indices].componentType, primitive.material, indexBufferId, positionBufferId, normalBufferId, uvBufferId);
			}
		}

		transform = glm::identity<glm::mat4x4>();
		transform = glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.0f, 1.0f, 0.0f));

		size_t separatorLocation = filepath.rfind('/') + 1;
		VoxLog(Display, Rendering, "Successfully loaded model '{}' with {} primitives.", filepath.substr(separatorLocation, filepath.size() - separatorLocation), meshes.size());
	}

	Model::~Model()
	{
		glDeleteBuffers(bufferIds.size(), bufferIds.data());
	}

	// @TODO: store uniforms in a more easily accessbile format?
	void Model::Render(Shader& shader, unsigned int modelUniformLocation, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation)
	{
		shader.SetUniformMatrix(modelUniformLocation, transform);
		// Assume our VAO is already bound?

		// Render primitives one at a time;

		for (const Mesh& mesh : meshes)
		{
			const PBRMaterial& material = materials[mesh.GetMaterialIndex()];
			shader.SetUniformColor(colorUniformLocation, material.albedo);
			shader.SetUniformFloat(roughnessUniformLocation, material.roughness);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBuffer());
			glBindVertexBuffer(0, mesh.GetPositionBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(1, mesh.GetNormalBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(2, mesh.GetUVBuffer(), 0, sizeof(float) * 2);
			glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), mesh.GetComponentType(), 0);
		}
	}

	glm::mat4x4 Model::GetMatrix() const
	{
		return transform;
	}
}