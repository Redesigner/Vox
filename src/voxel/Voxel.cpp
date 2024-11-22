#include "Voxel.h"

bool Voxel::operator==(const Voxel& voxel) const
{
	return (filled == voxel.filled) && (materialId == voxel.materialId);
}