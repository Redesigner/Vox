#include "RayCastResultNormal.h"

namespace Vox
{
	RayCastResultNormal::RayCastResultNormal()
	{
	}

	RayCastResultNormal::RayCastResultNormal(JPH::BodyID hitBody, JPH::Vec3 origin, JPH::Vec3 impactNormal, JPH::Vec3 impactPoint)
		:hitBody(hitBody), origin(origin), impactNormal(impactNormal), impactPoint(impactPoint)
	{
	}
}