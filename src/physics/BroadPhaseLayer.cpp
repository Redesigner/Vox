#include "BroadPhaseLayer.h"

#include "physics/ObjectLayerTypes.h"

const JPH::BroadPhaseLayer BroadPhaseLayerImplementation::objectBroadPhaseMap[BroadPhaseLayerImplementation::numLayers] =
{
	JPH::BroadPhaseLayer(0),
	JPH::BroadPhaseLayer(1)
};

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
