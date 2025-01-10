#pragma once

#include <memory>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "voxel/Voxel.h"

namespace Octree
{
	class Node;
}

class VoxelGrid
{
	struct Vbos
	{
		unsigned int position, texCoord, normal, textureId, index;
	};

public:
	VoxelGrid(unsigned int width, unsigned int height, unsigned int depth);
	~VoxelGrid();

	static std::shared_ptr<VoxelGrid> FromOctree(Octree::Node* octree);

	Voxel& GetVoxel(unsigned int x, unsigned int y, unsigned int z);
	const Voxel& GetVoxel(unsigned int x, unsigned int y, unsigned int z) const;

	void SetVoxel(Voxel voxel, unsigned int x, unsigned int y, unsigned int z);

	void GenerateMesh();

	unsigned int GetVertexCount() const;
	unsigned int* GetIndices();

	int x, y, z = 0;

private:
	void GenerateVertexObjects();

	void UnloadVertexObjects();

	void AddCube(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);

	void AddTopFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);
	void AddBottomFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);
	void AddFrontFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);
	void AddBackFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);
	void AddLeftFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);
	void AddRightFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId);

	unsigned int width, height, depth;

	static const glm::vec3 up;
	static const glm::vec3 down;
	static const glm::vec3 forward;
	static const glm::vec3 back;
	static const glm::vec3 left;
	static const glm::vec3 right;

	static const glm::vec2 topLeft;
	static const glm::vec2 topRight;
	static const glm::vec2 bottomLeft;
	static const glm::vec2 bottomRight;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> materialIds;
	std::vector<unsigned int> indices;

	std::vector<Voxel> voxels;

	unsigned int indexCount = 0;
	unsigned int vaoId = 0;
	Vbos vbos;

	bool meshCreated = false;
};