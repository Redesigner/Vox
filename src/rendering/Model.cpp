#include "Model.h"

#include <GL/glew.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

#include "core/logging/Logging.h"

namespace Vox
{
	Model::Model(std::string filepath)
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		model = std::make_unique<tinygltf::Model>();
		bool ret = loader.LoadBinaryFromFile(model.get(), &err, &warn, filepath);

		// Buffer our data into opengl
		// One GL buffer corresponds to a bufferview
		bufferIds = std::vector<unsigned int>(model->bufferViews.size(), 0);
		glCreateBuffers(model->bufferViews.size(), bufferIds.data());
		for (int i = 0; i < model->bufferViews.size(); ++i)
		{
			tinygltf::BufferView bufferView = model->bufferViews[i];
			glNamedBufferData(bufferIds[i], bufferView.byteLength, model->buffers[bufferView.buffer].data.data() + bufferView.byteOffset, GL_STATIC_DRAW);
		}

		for (const tinygltf::Mesh& mesh : model->meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
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

				const unsigned int indexCount = model->accessors[primitive.indices].count;
				const unsigned int indexBufferId = bufferIds[model->accessors[primitive.indices].bufferView];
				const unsigned int positionBufferId = bufferIds[model->accessors[positionBuffer->second].bufferView];
				const unsigned int normalBufferId = bufferIds[model->accessors[normalBuffer->second].bufferView];
				const unsigned int uvBufferId = bufferIds[model->accessors[uvBuffer->second].bufferView];
				meshes.emplace_back(indexCount, model->accessors[primitive.indices].componentType, indexBufferId, positionBufferId, normalBufferId, uvBufferId);
			}
		}
	}

	Model::~Model()
	{
		glDeleteBuffers(bufferIds.size(), bufferIds.data());
	}

	void Model::Render()
	{
		// Assume our VAO is already bound?

		// Render primitives one at a time;

		for (const Mesh& mesh : meshes)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBuffer());
			glBindVertexBuffer(0, mesh.GetPositionBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(1, mesh.GetNormalBuffer(), 0, sizeof(float) * 3);
			glBindVertexBuffer(2, mesh.GetUVBuffer(), 0, sizeof(float) * 2);
			glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), mesh.GetComponentType(), 0);
		}

		/*
		for (const tinygltf::Mesh& mesh : model->meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				// This is not in order...
				auto accessorIterator = primitive.attributes.cbegin();
				for (int i = 0; i < primitive.attributes.size(); ++i)
				{
					tinygltf::Accessor& accessor = model->accessors[accessorIterator->second];
					tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
					glBindVertexBuffer(i, bufferIds[accessor.bufferView], bufferView.byteOffset, bufferView.byteStride);
					++accessorIterator;
				}

				tinygltf::Accessor& indexAccessor = model->accessors[primitive.indices];
				tinygltf::BufferView& indexBufferView = model->bufferViews[indexAccessor.bufferView];
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[indexBufferView.buffer]);
				glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType, 0);
			}
		}
		*/
	}
}