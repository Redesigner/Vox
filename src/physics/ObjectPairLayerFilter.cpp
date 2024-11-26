#include "ObjectPairLayerFilter.h"

#include "physics/ObjectLayerTypes.h"

bool ObjectLayerPairFilterImplementation::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
	switch (inObject1)
	{
		case Physics::CollisionLayer::Dynamic:
		{
			return true;
		}
		case Physics::CollisionLayer::Static:
		{
			return inObject2 == Physics::CollisionLayer::Dynamic;
		}
	}
	return false;
}