#include "Model.h"

#include <GL/glew.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

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

		for (const tinygltf::BufferView& bufferView : model->bufferViews)
		{
		}

		for (const tinygltf::Accessor& accessor : model->accessors)
		{
		}

		for (const tinygltf::Mesh& mesh : model->meshes)
		{
		}
	}

	Model::~Model()
	{ }

	void Model::Render()
	{
		// Assume our VAO is already bound?

		// Render primitives one at a time;
		for (const tinygltf::Mesh& mesh : model->meshes)
		{
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				auto accessorIterator = primitive.attributes.cbegin();
				for (int i = 0; i < primitive.attributes.size(); ++i)
				{
					tinygltf::Accessor& accessor = model->accessors[accessorIterator->second];
					tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
					unsigned int bufferId = bufferIds[bufferView.buffer];
					glBindVertexBuffer(i, bufferId, bufferView.byteOffset, bufferView.byteStride);
					++accessorIterator;
				}

				tinygltf::Accessor& indexAccessor = model->accessors[primitive.indices];
				tinygltf::BufferView& indexBufferView = model->bufferViews[indexAccessor.bufferView];
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[indexBufferView.buffer]);
				glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType, 0);
			}
		}
	}
}