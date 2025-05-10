#include "Model.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tiny_gltf.h>

#include "core/logging/Logging.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/GBufferShader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/PickShader.h"

namespace Vox
{
	Model::Model(const std::string& filepath)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		tinygltf::Model model;
		loader.LoadBinaryFromFile(&model, &err, &warn, filepath);

		// Buffer our data into opengl
		// One GL buffer corresponds to a bufferview
		bufferIds = std::vector<unsigned int>(model.bufferViews.size(), 0);
		glCreateBuffers(static_cast<int>(model.bufferViews.size()), bufferIds.data());
		for (int i = 0; i < model.bufferViews.size(); ++i)
		{
			tinygltf::BufferView bufferView = model.bufferViews[i];
			glNamedBufferData(bufferIds[i], static_cast<GLsizei>(bufferView.byteLength), model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, GL_STATIC_DRAW);
		}

		for (const tinygltf::Material& material : model.materials)
		{
			const std::vector<double>& color = material.pbrMetallicRoughness.baseColorFactor;
			materials.emplace_back(
			    glm::vec4(color[0], color[1], color[2], color[3]),
			    static_cast<float>(material.pbrMetallicRoughness.roughnessFactor),
			    static_cast<float>(material.pbrMetallicRoughness.metallicFactor));
		}

		// initialize all our nodes, since we access them by index
		nodes = std::vector<ModelNode>(model.nodes.size());
		for (int i = 0; i < model.nodes.size(); ++i)
		{
			tinygltf::Node& node = model.nodes[i];
			ModelNode& newNode = nodes[i];
			newNode.localTransform = CalculateNodeTransform(node);
			newNode.children = node.children;
			newNode.mesh = node.mesh;
			for (int child : node.children)
			{
				// nodes can only be children of one node, so they should only ever be visited once
				nodes[child].root = false;
			}
		}

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			std::vector<unsigned int>& newMesh = meshes.emplace_back();
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
					VoxLog(Error, Rendering, "GLTF model does not have a tex coord attribute.");
					break;
				}

				const unsigned int indexCount = model.accessors[primitive.indices].count;
				const unsigned int indexBufferId = bufferIds[model.accessors[primitive.indices].bufferView];
				const unsigned int positionBufferId = bufferIds[model.accessors[positionBuffer->second].bufferView];
				const unsigned int normalBufferId = bufferIds[model.accessors[normalBuffer->second].bufferView];
				const unsigned int uvBufferId = bufferIds[model.accessors[uvBuffer->second].bufferView];
				Primitive& newPrimitive = primitives.emplace_back(indexCount, model.accessors[primitive.indices].componentType, primitive.material, indexBufferId, positionBufferId, normalBufferId, uvBufferId);
				newMesh.emplace_back(static_cast<unsigned int>(primitives.size() - 1)); // Store the primitive index so our nodes can find it later
			}
		}

		// Initialize our node transforms
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].root)
			{
				UpdateTransforms(i, glm::identity<glm::mat4x4>());
			}
		}

		size_t separatorLocation = filepath.rfind('/') + 1;
		VoxLog(Display, Rendering, "Successfully loaded model '{}' with {} primitives.", filepath.substr(separatorLocation, filepath.size() - separatorLocation), primitives.size());
	}

	Model::~Model()
	{
		glDeleteBuffers(static_cast<int>(bufferIds.size()), bufferIds.data());
	}

	void Model::Render(const GBufferShader* shader, const glm::mat4x4& rootMatrix)
	{
		// Assume our VAO is already bound?
		// Render primitives one at a time;

		for (const Primitive& primitive : primitives)
		{
			PBRMaterial& material = materials[primitive.GetMaterialIndex()];
			shader->SetMaterial(material);
			shader->SetModelMatrix(rootMatrix * primitive.GetTransform());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.GetIndexBuffer());
			glBindVertexBuffer(0, primitive.GetPositionBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(1, primitive.GetNormalBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(2, primitive.GetUVBuffer(), 0, sizeof(float) * 2);

			glDrawElements(GL_TRIANGLES, static_cast<int>(primitive.GetVertexCount()), primitive.GetComponentType(), nullptr);
		}
	}

    void Model::Render(const MeshShader* shader, const glm::mat4x4& rootMatrix) const
    {
	    for (const Primitive& primitive : primitives)
	    {
	        shader->SetModelMatrix(rootMatrix * primitive.GetTransform());

	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.GetIndexBuffer());
	        glBindVertexBuffer(0, primitive.GetPositionBuffer(), 0, sizeof(float) * 3);
	        glBindVertexBuffer(1, primitive.GetNormalBuffer(), 0, sizeof(float) * 3);
	        glBindVertexBuffer(2, primitive.GetUVBuffer(), 0, sizeof(float) * 2);

	        glDrawElements(GL_TRIANGLES, static_cast<int>(primitive.GetVertexCount()), primitive.GetComponentType(), nullptr);
	    }
    }

#ifdef EDITOR
	void Model::Render(const PickShader* shader, const int objectId, const glm::mat4x4& rootMatrix)
	{
		for (const auto& primitive : primitives)
		{
			shader->SetObjectId(objectId);
			shader->SetModelMatrix(rootMatrix * primitive.GetTransform());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.GetIndexBuffer());
			glBindVertexBuffer(0, primitive.GetPositionBuffer(), 0, sizeof(float) * 3);
			glDrawElements(GL_TRIANGLES, static_cast<int>(primitive.GetVertexCount()), primitive.GetComponentType(), nullptr);
		}
	}
#endif

	ModelTransform Model::CalculateNodeTransform(const tinygltf::Node& node) const
	{
		ModelTransform transform;
		if (node.matrix.empty())
		{
			if (node.translation.size() == 3)
			{
				transform.position = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
			}

			if (node.rotation.size() == 4)
			{
				transform.rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			}

			if (node.scale.size() == 3)
			{
				transform.scale =  glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
			}
		}
		else
		{
			transform = ModelTransform(glm::mat4x4(
				node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
				node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
				node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
				node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
			));
		}

		return transform;
	}

	void Model::UpdateTransforms(unsigned int nodeIndex, glm::mat4x4 transform)
	{
		if (nodeIndex >= nodes.size())
		{
			return;
		}

		ModelNode& node = nodes[nodeIndex];
		node.globalTransform = transform * node.localTransform.GetMatrix();
		if (node.mesh >= 0)
		{
			for (int primitiveIndex : meshes[node.mesh])
			{
				primitives[primitiveIndex].SetTransform(node.globalTransform);
			}
		}

		for (int nodeIndex : node.children)
		{
			UpdateTransforms(nodeIndex, node.globalTransform);
		}
	}
}
