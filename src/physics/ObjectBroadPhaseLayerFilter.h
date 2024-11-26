#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

class ObjectVsBroadPhaseLayerFilterImplementation : public JPH::ObjectVsBroadPhaseLayerFilter
{
	bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};