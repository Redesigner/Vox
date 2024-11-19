#pragma once

#include "voxel/Voxel.h"
#include "raylib.h"

#include <vector>

class VoxelGrid
{
public:
	VoxelGrid(unsigned int width, unsigned int height, unsigned int depth);

	Voxel& GetVoxel(unsigned int x, unsigned int y, unsigned int z);
	const Voxel& GetVoxel(unsigned int x, unsigned int y, unsigned int z) const;

	void SetVoxel(Voxel voxel, unsigned int x, unsigned int y, unsigned int z);

	Mesh GenerateMesh();

private:
	void AddCube(unsigned int x, unsigned int y, unsigned int z);

	void AddTopFace(unsigned int x, unsigned int y, unsigned int z);
	void AddBottomFace(unsigned int x, unsigned int y, unsigned int z);
	void AddFrontFace(unsigned int x, unsigned int y, unsigned int z);
	void AddBackFace(unsigned int x, unsigned int y, unsigned int z);
	void AddLeftFace(unsigned int x, unsigned int y, unsigned int z);
	void AddRightFace(unsigned int x, unsigned int y, unsigned int z);

	unsigned int width, height, depth;

	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 forward;
	static const Vector3 back;
	static const Vector3 left;
	static const Vector3 right;

	static const Vector2 topLeft;
	static const Vector2 topRight;
	static const Vector2 bottomLeft;
	static const Vector2 bottomRight;

	std::vector<Vector3> vertices;
	std::vector<Vector2> texCoords;
	std::vector<Vector3> normals;
	std::vector<unsigned short> indices;

	std::vector<Voxel> voxels;
};