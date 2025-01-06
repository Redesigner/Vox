#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Vox
{
	// A raycast result that holds all of our information, including the normal that was hit by our ray
	struct RayCastResultNormal
	{
		RayCastResultNormal();
		RayCastResultNormal(JPH::BodyID hitBody, JPH::Vec3 origin, JPH::Vec3 impactNormal, JPH::Vec3 impactPoint);

		JPH::BodyID hitBody;
		JPH::Vec3 origin;
		JPH::Vec3 impactNormal;
		JPH::Vec3 impactPoint;
	};
}