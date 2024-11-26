#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/ObjectLayer.h>

class ObjectLayerPairFilterImplementation : public JPH::ObjectLayerPairFilter
{
	bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};