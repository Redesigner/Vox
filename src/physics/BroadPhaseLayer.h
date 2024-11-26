#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

class BroadPhaseLayerImplementation final : public JPH::BroadPhaseLayerInterface
{
	JPH::uint GetNumBroadPhaseLayers() const override;

	JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;


	static const unsigned int numLayers = 4;
	JPH::BroadPhaseLayer objectBroadPhaseMap[numLayers];
};
