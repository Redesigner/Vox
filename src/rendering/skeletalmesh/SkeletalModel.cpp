#include "SkeletalModel.h"

#include <ranges>

#include <fmt/format.h>
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tiny_gltf.h>

#include "core/logging/Logging.h"
#include "rendering/shaders/Shader.h"

namespace Vox
{
	SkeletalModel::SkeletalModel(std::string filepath)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		tinygltf::Model model;
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);

		std::vector<unsigned int> meshBuffers = GetMeshBuffers(model);
		// Dump our buffer ids here, so we can grab them in order from the buffered data
		std::vector<unsigned int> tempBufferLookup = std::vector<unsigned int>(model.bufferViews.size());

		bufferIds = std::vector<unsigned int>(meshBuffers.size(), 0);
		glCreateBuffers(meshBuffers.size(), bufferIds.data());
		for (int i = 0; i < meshBuffers.size(); ++i)
		{
			tempBufferLookup[meshBuffers[i]] = bufferIds[i];
			tinygltf::BufferView bufferView = model.bufferViews[meshBuffers[i]];
			glNamedBufferData(bufferIds[i], bufferView.byteLength, model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, GL_STATIC_DRAW);
		}

		for (const tinygltf::Animation& animation : model.animations)
		{
			animations.try_emplace(animation.name, animation, model);
		}

		// Log our loaded animation names
		std::string animationNames;
		for (const std::pair<std::string, Animation>& animation : animations)
		{
			animationNames.append(fmt::format("[{} : {}s]", animation.first, animation.second.GetDuration()));
			//if (animation != *--animations.end())
			{
				animationNames.append(", ");
			}
		}
		VoxLog(Display, Rendering, "Skeletal mesh loaded {} animations: \n\t'{{ {} }}'", animations.size(), animationNames);


		for (const tinygltf::Material& material : model.materials)
		{
			const std::vector<double>& color = material.pbrMetallicRoughness.baseColorFactor;
			materials.emplace_back(glm::vec4(color[0], color[1], color[2], color[3]), material.pbrMetallicRoughness.roughnessFactor, material.pbrMetallicRoughness.metallicFactor);
		}

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
				nodes[child].root = false;
			}
		}

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			std::vector<unsigned int>& newMesh = meshes.emplace_back();
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				if (primitive.attributes.size() > 5)
				{
					VoxLog(Warning, Rendering, "GLTF skeletal model has more than five attributes. Skeletal models currently only support having POSITION, NORMAL, TEXCOORD_0, JOINTS_0, and WEIGHTS_0.");
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

				auto jointsBuffer = primitive.attributes.find("JOINTS_0");
				if (jointsBuffer == primitive.attributes.end())
				{
					VoxLog(Error, Rendering, "GLTF model does not have a joints attribute.");
					break;
				}

				auto weightsBuffer = primitive.attributes.find("WEIGHTS_0");
				if (weightsBuffer == primitive.attributes.end())
				{
					VoxLog(Error, Rendering, "GLTF model does not have a weights attribute.");
					break;
				}

				// There are a couple layers of indirection going on here:
				// All of our openGL buffer ids are stored in bufferIds
				// TempBufferLookup simulates the buffer view list, since bufferviews are ordered and
				// accessors retrieve these bufferviews by index
				SkeletalPrimitive& newPrimitive = primitives.emplace_back();
				newPrimitive.vertexCount = model.accessors[primitive.indices].count;
				newPrimitive.indexBuffer = tempBufferLookup[model.accessors[primitive.indices].bufferView];
				newPrimitive.positionBuffer = tempBufferLookup[model.accessors[positionBuffer->second].bufferView];
				newPrimitive.normalBuffer = tempBufferLookup[model.accessors[normalBuffer->second].bufferView];
				newPrimitive.uvBuffer = tempBufferLookup[model.accessors[uvBuffer->second].bufferView];
				newPrimitive.jointsBuffer = tempBufferLookup[model.accessors[jointsBuffer->second].bufferView];
				newPrimitive.weightsBuffer = tempBufferLookup[model.accessors[weightsBuffer->second].bufferView];
				newPrimitive.componentType = model.accessors[primitive.indices].componentType;
				newMesh.emplace_back(primitives.size() - 1);
			}
		}

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

	SkeletalModel::~SkeletalModel()
	{
		glDeleteBuffers(bufferIds.size(), bufferIds.data());
	}

	void SkeletalModel::Render(Shader& shader, unsigned int modelUniformLocation, glm::mat4x4 transform, unsigned int colorUniformLocation, unsigned int roughnessUniformLocation)
	{
		for (const SkeletalPrimitive& primitive : primitives)
		{
			const PBRMaterial& material = materials[primitive.materialIndex];
			shader.SetUniformColor(colorUniformLocation, material.albedo);
			shader.SetUniformFloat(roughnessUniformLocation, material.roughness);

			shader.SetUniformMatrix(modelUniformLocation, transform);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.indexBuffer);
			glBindVertexBuffer(0, primitive.positionBuffer, 0, sizeof(float) * 3);
			glBindVertexBuffer(1, primitive.normalBuffer, 0, sizeof(float) * 3);
			glBindVertexBuffer(2, primitive.uvBuffer, 0, sizeof(float) * 2);
			glBindVertexBuffer(3, primitive.jointsBuffer, 0, sizeof(float) * 4);
			glBindVertexBuffer(4, primitive.weightsBuffer, 0, sizeof(float) * 4);

			glDrawElements(GL_TRIANGLES, primitive.vertexCount, primitive.componentType, 0);
		}
	}

	Transform SkeletalModel::CalculateNodeTransform(const tinygltf::Node& node) const
	{
		Transform transform;
		if (node.matrix.size() == 0)
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
				transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
			}
		}
		else
		{
			transform = Transform(glm::mat4x4(
				node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
				node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
				node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
				node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
			));
		}

		return transform;
	}

	void SkeletalModel::UpdateTransforms(unsigned int nodeIndex, glm::mat4x4 transform)
	{
		if (nodeIndex >= nodes.size())
		{
			return;
		}

		ModelNode& node = nodes[nodeIndex];
		node.globalTransform = transform * node.localTransform.GetMatrix();

		for (int nodeIndex : node.children)
		{
			UpdateTransforms(nodeIndex, node.globalTransform);
		}
	}

	// Calculate which bufferViews from the model need to be uploaded to the GPU
	// this should only be primitive attributes -- bufferViews used in animation
	// should be stored on the CPU instead
	std::vector<unsigned int> SkeletalModel::GetMeshBuffers(const tinygltf::Model& model) const
	{
		std::vector<unsigned int> meshBuffers;

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				auto positionBuffer = primitive.attributes.find("POSITION");
				if (positionBuffer == primitive.attributes.end())
				{
					break;
				}

				auto normalBuffer = primitive.attributes.find("NORMAL");
				if (normalBuffer == primitive.attributes.end())
				{
					break;
				}

				auto uvBuffer = primitive.attributes.find("TEXCOORD_0");
				if (uvBuffer == primitive.attributes.end())
				{
					break;
				}

				auto jointsBuffer = primitive.attributes.find("JOINTS_0");
				if (jointsBuffer == primitive.attributes.end())
				{
					break;
				}

				auto weightsBuffer = primitive.attributes.find("WEIGHTS_0");
				if (weightsBuffer == primitive.attributes.end())
				{
					break;
				}

				meshBuffers.emplace_back(positionBuffer->second);
				meshBuffers.emplace_back(normalBuffer->second);
				meshBuffers.emplace_back(uvBuffer->second);
				meshBuffers.emplace_back(jointsBuffer->second);
				meshBuffers.emplace_back(weightsBuffer->second);
				meshBuffers.emplace_back(primitive.indices);
			}
		}
		return meshBuffers;
	}
}