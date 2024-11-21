#include "VoxelGrid.h"

#include "rlgl.h"
#include "external/glad.h"

#include <stdexcept>


const Vector3 VoxelGrid::up = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 VoxelGrid::down = Vector3(0.0f, -1.0f, 0.0f);
const Vector3 VoxelGrid::forward = Vector3(0.0f, 0.0f, 1.0f);
const Vector3 VoxelGrid::back = Vector3(0.0f, 0.0f, -1.0f);
const Vector3 VoxelGrid::left = Vector3(-1.0f, 0.0f, 0.0f);
const Vector3 VoxelGrid::right = Vector3(1.0f, 0.0f, 0.0f);

const Vector2 VoxelGrid::topLeft = Vector2(0.0f, 1.0f);
const Vector2 VoxelGrid::topRight = Vector2(1.0f, 1.0f);
const Vector2 VoxelGrid::bottomLeft = Vector2(0.0f, 0.0f);
const Vector2 VoxelGrid::bottomRight = Vector2(0.0f, 1.0f);

VoxelGrid::VoxelGrid(unsigned int width, unsigned int height, unsigned int depth)
	:width(width), height(height), depth(depth)
{
	voxels = std::vector<Voxel>(width * height * depth);

	vaoId = rlLoadVertexArray();
	vbos = {};
}

VoxelGrid::~VoxelGrid()
{
	UnloadVertexObjects();
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
	UnloadVertexObjects();

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
	vaoId = rlLoadVertexArray();
	EnableVertexArray();

	// Position
	vbos.position = rlLoadVertexBuffer(&vertices[0], sizeof(Vector3) * vertices.size(), dynamic);
	rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 0, 0);
	rlEnableVertexAttribute(0);
	vertices.clear();

	// TexCoord
	vbos.texCoord = rlLoadVertexBuffer(&texCoords[0], sizeof(Vector2) * texCoords.size(), dynamic);
	rlSetVertexAttribute(1, 2, RL_FLOAT, 0, 0, 0);
	rlEnableVertexAttribute(1);
	texCoords.clear();

	// Normal
	vbos.normal = rlLoadVertexBuffer(&normals[0], sizeof(Vector3) * normals.size(), dynamic);
	rlSetVertexAttribute(2, 3, RL_FLOAT, 0, 0, 0);
	rlEnableVertexAttribute(2);
	normals.clear();

	// TextureId
	vbos.textureId = rlLoadVertexBuffer(&materialIds[0], sizeof(unsigned int) * materialIds.size(), dynamic);
	// rlSetVertexAttribute(3, 1, GL_UNSIGNED_INT, 0, 0, 0);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, 0);
	rlEnableVertexAttribute(3);
	materialIds.clear();

	// Indices
	vbos.index = rlLoadVertexBufferElement(&indices[0], sizeof(unsigned short) * indices.size(), dynamic);
	indexCount = indices.size();

	rlDisableVertexArray();
}

void VoxelGrid::EnableVertexArray()
{
	rlEnableVertexArray(vaoId);
}

unsigned int VoxelGrid::GetVertexCount() const
{
	return indexCount;
}

unsigned short* VoxelGrid::GetIndices()
{
	return &indices[0];
}

void VoxelGrid::UnloadVertexObjects()
{
	if (vbos.position)
	{
		rlUnloadVertexBuffer(vbos.position);
		vbos.position = 0;
	}
	if (vbos.texCoord)
	{
		rlUnloadVertexBuffer(vbos.texCoord);
		vbos.texCoord = 0;
	}
	if (vbos.normal)
	{
		rlUnloadVertexBuffer(vbos.normal);
		vbos.normal = 0;
	}
	if (vbos.position)
	{
		rlUnloadVertexBuffer(vbos.textureId);
		vbos.textureId = 0;
	}
	if (vbos.index)
	{
		rlUnloadVertexBuffer(vbos.index);
		vbos.index = 0;
	}

	if (vaoId)
	{
		rlUnloadVertexArray(vaoId);
		vaoId = 0;
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
