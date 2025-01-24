#pragma once

struct Voxel
{
	unsigned int materialId = 0;

	bool operator == (const Voxel& voxel) const;
};