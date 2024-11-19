#include "VoxelGrid.h"

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

Mesh VoxelGrid::GenerateMesh()
{
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				if (voxels[static_cast<size_t>(x + (width * y) + (width * height * z))].filled)
				{
					AddCube(x, y, z);
				}
			}
		}
	}

	Mesh mesh{ 0 };
	mesh.vertices = reinterpret_cast<float*>(&vertices[0]);
	mesh.vertexCount = vertices.size();
	vertices.clear();

	mesh.texcoords = reinterpret_cast<float*>(&texCoords[0]);
	texCoords.clear();

	mesh.normals = reinterpret_cast<float*>(&normals[0]);
	normals.clear();

	mesh.indices = &indices[0];
	mesh.triangleCount = indices.size() / 3;
	indices.clear();

	return mesh;
}

void VoxelGrid::AddCube(unsigned int x, unsigned int y, unsigned int z)
{
	AddTopFace(x, y, z);
	AddBottomFace(x, y, z);
	AddFrontFace(x, y, z);
	AddBackFace(x, y, z);
	AddLeftFace(x, y, z);
	AddRightFace(x, y, z);
}

void VoxelGrid::AddTopFace(unsigned int x, unsigned int y, unsigned int z)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size() + 1;
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

	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 2);
}

void VoxelGrid::AddBottomFace(unsigned int x, unsigned int y, unsigned int z)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);
	float zf = static_cast<float>(z);

	int currentIndex = vertices.size() + 1;
	vertices.emplace_back(xf,			yf,	zf);
	vertices.emplace_back(xf + 1.0f,	yf,	zf);
	vertices.emplace_back(xf,			yf,	zf + 1.0f);
	vertices.emplace_back(xf + 1.0f,	yf,	zf + 1.0f);

	texCoords.emplace_back(topLeft);
	texCoords.emplace_back(topRight);
	texCoords.emplace_back(bottomLeft);
	texCoords.emplace_back(bottomRight);

	normals.emplace_back(up);
	normals.emplace_back(up);
	normals.emplace_back(up);
	normals.emplace_back(up);

	indices.emplace_back(currentIndex);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 2);
	indices.emplace_back(currentIndex + 1);
	indices.emplace_back(currentIndex + 3);
	indices.emplace_back(currentIndex + 2);
}

void VoxelGrid::AddFrontFace(unsigned int x, unsigned int y, unsigned int z)
{
}

void VoxelGrid::AddBackFace(unsigned int x, unsigned int y, unsigned int z)
{
}

void VoxelGrid::AddLeftFace(unsigned int x, unsigned int y, unsigned int z)
{
}

void VoxelGrid::AddRightFace(unsigned int x, unsigned int y, unsigned int z)
{
}
