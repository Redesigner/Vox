#include "ObjectBroadPhaseLayerFilter.h"

bool ObjectVsBroadPhaseLayerFilterImplementation::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	return true;
}
