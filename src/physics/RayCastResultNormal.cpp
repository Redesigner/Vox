#include "RayCastResultNormal.h"

namespace Vox
{
	RayCastResultNormal::RayCastResultNormal()
        :hitBody(JPH::BodyID()), origin({0.0f}), impactNormal({0.0f}), impactPoint({0.0f}), percentage(1.0f)
	{
	}

	RayCastResultNormal::RayCastResultNormal(JPH::BodyID hitBody, JPH::Vec3 origin, JPH::Vec3 impactNormal, JPH::Vec3 impactPoint, float percentage)
		:hitBody(hitBody), origin(origin), impactNormal(impactNormal), impactPoint(impactPoint), percentage(percentage)
	{
	}
}