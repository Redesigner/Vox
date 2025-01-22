#pragma once

struct Voxel
{
	unsigned short materialId = 0;

	bool operator == (const Voxel& voxel) const;
};