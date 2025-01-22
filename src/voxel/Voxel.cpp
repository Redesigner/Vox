#include "Voxel.h"

bool Voxel::operator==(const Voxel& voxel) const
{
	return (materialId == voxel.materialId);
}