#include "VoxelGrid.h"

#include <stdexcept>

#include <GL/glew.h>

#include "voxel/Octree.h"


const glm::vec3 VoxelGrid::up = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 VoxelGrid::down = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 VoxelGrid::forward = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 VoxelGrid::back = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 VoxelGrid::left = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::vec3 VoxelGrid::right = glm::vec3(1.0f, 0.0f, 0.0f);

const glm::vec2 VoxelGrid::topLeft = glm::vec2(0.0f, 1.0f);
const glm::vec2 VoxelGrid::topRight = glm::vec2(1.0f, 1.0f);
const glm::vec2 VoxelGrid::bottomLeft = glm::vec2(0.0f, 0.0f);
const glm::vec2 VoxelGrid::bottomRight = glm::vec2(0.0f, 1.0f);

VoxelGrid::VoxelGrid(unsigned int width, unsigned int height, unsigned int depth)
	:width(width), height(height), depth(depth), x(0), y(0), z(0)
{
	voxels = std::vector<Voxel>(width * height * depth);

	vaoId = 0;
	vbos = {};

	GenerateVertexObjects();
}

VoxelGrid::~VoxelGrid()
{
	UnloadVertexObjects();
}

std::shared_ptr<VoxelGrid> VoxelGrid::FromOctree(Octree::Node* octree)
{
	unsigned int size = octree->GetSize();
	unsigned int half = size / 2;

	std::shared_ptr<VoxelGrid> voxelGrid = std::make_shared<VoxelGrid>(size, size, size);

	for (int x = 0; x < size; ++x)
	{
		for (int y = 0; y < size; ++y)
		{
			for (int z = 0; z < size; ++z)
			{
				Voxel* voxel = octree->GetVoxel(x - half, y - half, z - half);
				if (voxel)
				{
					voxelGrid->GetVoxel(x, y, z) = *voxel;
				}
			}
		}
	}
	return voxelGrid;
}

Voxel& VoxelGrid::GetVoxel(unsigned int x, unsigned int y, unsigned int z)
{
	if (x >= width)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (y >= height)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (z >= depth)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}

	return voxels[static_cast<size_t>(x + (width * y) + (width * height * z))];
}

const Voxel& VoxelGrid::GetVoxel(unsigned int x, unsigned int y, unsigned int z) const
{
	if (x >= width)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (y >= height)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (z >= depth)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}

	return voxels[static_cast<size_t>(x + (width * y) + (width * height * z))];
}

void VoxelGrid::SetVoxel(Voxel voxel, unsigned int x, unsigned int y, unsigned int z)
{
	if (x >= width)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (y >= height)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}
	if (z >= depth)
	{
		throw std::out_of_range("Attempting to access VoxelGrid out of range.");
	}

	voxels[static_cast<size_t>(x + (width * y) + (width * height * z))] = voxel;
}

void VoxelGrid::GenerateMesh()
{
	indices.clear();

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				const Voxel& voxel = voxels[static_cast<size_t>(x + (width * y) + (width * height * z))];
				if (voxel.filled)
				{
					AddCube(x, y, z, voxel.materialId);
				}
			}
		}
	}
	
	// layout location values / indices here come from gBufferVoxel.vert
	// all vector sizes should be the same
	bool dynamic = false;
	glBindBuffer(GL_ARRAY_BUFFER, vbos.position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.texCoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), &texCoords[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.textureId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * materialIds.size(), &materialIds[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	vertices.clear();
	texCoords.clear();
	normals.clear();
	materialIds.clear();
	indexCount = indices.size();
}

unsigned int VoxelGrid::GetVertexCount() const
{
	return indexCount;
}

unsigned int* VoxelGrid::GetIndices()
{
	return &indices[0];
}

void VoxelGrid::GenerateVertexObjects()
{
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	unsigned int vboIds[5] = {};
	glGenBuffers(5, vboIds);
	vbos.position = vboIds[0];
	vbos.texCoord = vboIds[1];
	vbos.normal = vboIds[2];
	vbos.textureId = vboIds[3];
	vbos.index = vboIds[4];

	glBindBuffer(GL_ARRAY_BUFFER, vbos.position);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.texCoord);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.normal);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbos.textureId);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, 0);
	glEnableVertexAttribArray(3);

	meshCreated = true;
}

void VoxelGrid::UnloadVertexObjects()
{
	if (meshCreated)
	{
		meshCreated = false;
		unsigned int bufferIds[5] = { vbos.position, vbos.texCoord, vbos.normal, vbos.textureId, vbos.index };
		glDeleteBuffers(5, bufferIds);
		glDeleteVertexArrays(1, &vaoId);
	}
}

void VoxelGrid::AddCube(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	AddTopFace(x, y, z, materialId);
	AddBottomFace(x, y, z, materialId);
	AddFrontFace(x, y, z, materialId);
	AddBackFace(x, y, z, materialId);
	AddLeftFace(x, y, z, materialId);
	AddRightFace(x, y, z, materialId);
}

void VoxelGrid::AddTopFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf,			yf + 1.0f,	zf);
	vertices.emplace_back(xf + 1.0f,	yf + 1.0f,	zf);
	vertices.emplace_back(xf,			yf + 1.0f,	zf + 1.0f);
	vertices.emplace_back(xf + 1.0f,	yf + 1.0f,	zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(up);
	normals.emplace_back(up);
	normals.emplace_back(up);
	normals.emplace_back(up);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 1);
}

void VoxelGrid::AddBottomFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf,			yf,	zf);
	vertices.emplace_back(xf + 1.0f,	yf,	zf);
	vertices.emplace_back(xf,			yf,	zf + 1.0f);
	vertices.emplace_back(xf + 1.0f,	yf,	zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(down);
	normals.emplace_back(down);
	normals.emplace_back(down);
	normals.emplace_back(down);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 2);
}

void VoxelGrid::AddFrontFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf, yf, zf + 1.0f);
	vertices.emplace_back(xf + 1.0f, yf, zf + 1.0f);
	vertices.emplace_back(xf, yf + 1.0f, zf + 1.0f);
	vertices.emplace_back(xf + 1.0f, yf + 1.0f, zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(forward);
	normals.emplace_back(forward);
	normals.emplace_back(forward);
	normals.emplace_back(forward);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 2);
}

void VoxelGrid::AddBackFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf, yf, zf);
	vertices.emplace_back(xf + 1.0f, yf, zf);
	vertices.emplace_back(xf, yf + 1.0f, zf);
	vertices.emplace_back(xf + 1.0f, yf + 1.0f, zf);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(back);
	normals.emplace_back(back);
	normals.emplace_back(back);
	normals.emplace_back(back);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 1);
}

void VoxelGrid::AddLeftFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf, yf, zf);
	vertices.emplace_back(xf, yf + 1.0f, zf);
	vertices.emplace_back(xf, yf, zf + 1.0f);
	vertices.emplace_back(xf, yf + 1.0f, zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(left);
	normals.emplace_back(left);
	normals.emplace_back(left);
	normals.emplace_back(left);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 1);
}

void VoxelGrid::AddRightFace(unsigned int x, unsigned int y, unsigned int z, unsigned int materialId)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size();
	vertices.emplace_back(xf + 1.0f, yf, zf);
	vertices.emplace_back(xf + 1.0f, yf + 1.0f, zf);
	vertices.emplace_back(xf + 1.0f, yf, zf + 1.0f);
	vertices.emplace_back(xf + 1.0f, yf + 1.0f, zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(right);
	normals.emplace_back(right);
	normals.emplace_back(right);
	normals.emplace_back(right);

	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);
	materialIds.emplace_back(materialId);

	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 2);
}
