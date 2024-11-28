#include "BroadPhaseLayer.h"

JPH::uint BroadPhaseLayerImplementation::GetNumBroadPhaseLayers() const
{
	return numLayers;
}

JPH::BroadPhaseLayer BroadPhaseLayerImplementation::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
	return objectBroadPhaseMap[inLayer];
}

const char* BroadPhaseLayerImplementation::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
{
	return "nullptr";
}
