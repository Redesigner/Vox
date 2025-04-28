#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

class BroadPhaseLayerImplementation final : public JPH::BroadPhaseLayerInterface
{
	JPH::uint GetNumBroadPhaseLayers() const override;

	JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

	//const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;


	static const unsigned int numLayers = 2;
	static const JPH::BroadPhaseLayer objectBroadPhaseMap[numLayers];
};
