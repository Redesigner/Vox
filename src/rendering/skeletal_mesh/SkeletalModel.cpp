#include "SkeletalModel.h"

#include <ranges>

#include <fmt/format.h>
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tiny_gltf.h>

#include "core/logging/Logging.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/pixel_shaders/mesh_shaders/GBufferShader.h"

namespace Vox
{
	SkeletalModel::SkeletalModel(const std::string& filepath)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		tinygltf::Model model;
	    matrixBuffer = 0;
		loader.LoadBinaryFromFile(&model, &err, &warn, filepath);

		std::vector<unsigned int> meshBuffers = GetMeshBuffers(model);
		// Dump our buffer ids here, so we can grab them in order from the buffered data
		std::vector<unsigned int> tempBufferLookup = std::vector<unsigned int>(model.bufferViews.size());

		bufferIds = std::vector<unsigned int>(meshBuffers.size(), 0);
		glCreateBuffers(static_cast<int>(meshBuffers.size()), bufferIds.data());
		for (int i = 0; i < meshBuffers.size(); ++i)
		{
			tempBufferLookup[meshBuffers[i]] = bufferIds[i];
			tinygltf::BufferView bufferView = model.bufferViews[meshBuffers[i]];
			glNamedBufferData(bufferIds[i], static_cast<int>(bufferView.byteLength), model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset, GL_STATIC_DRAW);
		}

		// Allocate space for up to 'maxMatrixCount' matrix objects in our UBO
		glCreateBuffers(1, &matrixBuffer);
		glNamedBufferStorage(matrixBuffer, sizeof(glm::mat4x4) * maxMatrixCount, nullptr, GL_DYNAMIC_STORAGE_BIT);

		for (const tinygltf::Animation& animation : model.animations)
		{
			animations.try_emplace(animation.name, animation, model);
		}

		// Log our loaded animation names
		std::string animationNames;
		for (const auto& [name, animation] : animations)
		{
			animationNames.append(fmt::format("[{} : {}s]", name, animation.GetDuration()));
			//if (animation != *--animations.end())
			{
				animationNames.append(", ");
			}
		}
		VoxLog(Display, Rendering, "Skeletal mesh loaded {} animations: \n\t'{{ {} }}'", animations.size(), animationNames);

		// Create materials
		for (const tinygltf::Material& material : model.materials)
		{
			const std::vector<double>& color = material.pbrMetallicRoughness.baseColorFactor;
			materials.emplace_back(glm::vec4(color[0], color[1], color[2], color[3]), material.pbrMetallicRoughness.roughnessFactor, material.pbrMetallicRoughness.metallicFactor);
		}


		// Create nodes
		nodes = std::vector<ModelNode>(model.nodes.size());
		for (int i = 0; i < model.nodes.size(); ++i)
		{
			tinygltf::Node& node = model.nodes[i];
			ModelNode& newNode = nodes[i];
			newNode.localTransform = CalculateNodeTransform(node);
			newNode.children = node.children;
			newNode.mesh = node.mesh;
			newNode.name = node.name;
			for (int child : node.children)
			{
				nodes[child].root = false;
			}
		}

		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].root)
			{
				rootNodes.emplace_back(i);
			}
		}

		// Create temporary skins array, which will be copied into the primitives directly
		// during node creation
		skins = std::vector<std::vector<glm::mat4x4>>();
		for (const tinygltf::Skin& skin : model.skins)
		{
			const tinygltf::BufferView& inverseBindBufferView = model.bufferViews[model.accessors[skin.inverseBindMatrices].bufferView];
			std::vector<glm::mat4x4>& inverseBindMatrices = skins.emplace_back();
			tinygltf::Buffer& buffer = model.buffers[inverseBindBufferView.buffer];
			std::copy(
				reinterpret_cast<glm::mat4x4*>(buffer.data.data() + inverseBindBufferView.byteOffset),
				reinterpret_cast<glm::mat4x4*>(buffer.data.data() + inverseBindBufferView.byteOffset + inverseBindBufferView.byteLength),
				std::back_inserter(inverseBindMatrices)
			);

			joints = skin.joints;
		}

		for (int i = 0; i < skins.size(); ++i)
		{
			std::vector<glm::mat4x4>& inverseBindMatrices = skins[i];
			const tinygltf::Skin& skin = model.skins[i];

			for (int j = 0; j < inverseBindMatrices.size(); ++j)
			{
				ModelNode& node = nodes[skin.joints[j]];
				node.inverseBindMatrix = inverseBindMatrices[j];
			}
		}

		// Create primitives
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
				newPrimitive.materialIndex = primitive.material;
				newMesh.emplace_back(primitives.size() - 1);
			}
		}

		for (const unsigned int node : rootNodes)
		{
			UpdateTransforms(node, glm::identity<glm::mat4x4>());
		}

		const size_t separatorLocation = filepath.rfind('/') + 1;
		VoxLog(Display, Rendering, "Successfully loaded model '{}' with {} primitives.", filepath.substr(separatorLocation, filepath.size() - separatorLocation), primitives.size());
	}

	SkeletalModel::~SkeletalModel()
	{
		glDeleteBuffers(static_cast<int>(bufferIds.size()), bufferIds.data());
	}

	void SkeletalModel::Render(GBufferShader* shader, glm::mat4x4 transform)
	{
		//SetAnimation("Translate", currentAnimTime);
		//SetAnimation("Rotate", currentAnimTime);
		SetAnimation("StingerShot", currentAnimTime);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrixBuffer, 0, nodes.size() * sizeof(glm::mat4x4));

		for (const SkeletalPrimitive& primitive : primitives)
		{
		    shader->SetMaterial(materials[primitive.materialIndex]);
			shader->SetModelMatrix(transform);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.indexBuffer);
			glBindVertexBuffer(0, primitive.positionBuffer, 0, sizeof(float) * 3);
			glBindVertexBuffer(1, primitive.normalBuffer, 0, sizeof(float) * 3);
			glBindVertexBuffer(2, primitive.uvBuffer, 0, sizeof(float) * 2);
			glBindVertexBuffer(3, primitive.jointsBuffer, 0, sizeof(unsigned char) * 4);
			glBindVertexBuffer(4, primitive.weightsBuffer, 0, sizeof(float) * 4);

			glDrawElements(GL_TRIANGLES, primitive.vertexCount, primitive.componentType, 0);
		}

		currentAnimTime += 1.0f / 60.0f;
		if (currentAnimTime >= 2.5f)
		{
			currentAnimTime = 0.0f;
		}
	}

	void SkeletalModel::SetAnimation(std::string animationName, float time)
	{
		auto animationLookup = animations.find(animationName);
		if (animationLookup == animations.end())
		{
			VoxLog(Warning, Rendering, "Could not find animation '{}'", animationName);
			return;
		}

		Animation& animation = animationLookup->second;
		animation.ApplyToNodes(nodes, time);

		for (const unsigned int node : rootNodes)
		{
			UpdateTransforms(node, glm::identity<glm::mat4x4>());
		}

		// @TODO: handle multiple skins -- even if blender doesn't necessarily export these
		std::vector<glm::mat4x4> transforms;
		for (int i = 0; i < joints.size(); ++i)
		{
			ModelNode& node = nodes[joints[i]];
			transforms.emplace_back(node.globalTransform * node.inverseBindMatrix);
		}
		glNamedBufferSubData(matrixBuffer, 0, transforms.size() * sizeof(glm::mat4x4), transforms.data());
	}

	const std::unordered_map<std::string, Animation>& SkeletalModel::GetAnimations() const
	{
		return animations;
	}

	ModelTransform SkeletalModel::CalculateNodeTransform(const tinygltf::Node& node) const
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
				transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
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
