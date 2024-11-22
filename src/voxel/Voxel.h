#pragma once

struct Voxel
{
	bool filled = false;
	unsigned short materialId = 0;

	bool operator == (const Voxel& voxel) const;
};