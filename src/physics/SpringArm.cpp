#include "SpringArm.h"

#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "physics/PhysicsServer.h"

namespace Vox
{
	void SpringArm::SetOrigin(JPH::BodyID bodyId)
	{
		originType = OriginType::Body;
		originBody = bodyId;
	}

	void SpringArm::SetOrigin(unsigned int characterControllerId)
	{
		originType = OriginType::Character;
		originCharacterId = characterControllerId;
	}

	void SpringArm::SetEulerRotation(JPH::Vec3 rotation)
	{
		eulerRotation = rotation;
		JPH::Mat44 rotationMatrix = JPH::Mat44::sRotation(JPH::Quat::sEulerAngles(rotation));
		springVector = rotationMatrix * JPH::Vec3::sAxisZ() * springLength;
	}

	JPH::Vec3 SpringArm::GetEulerRotation() const
	{
		return eulerRotation;
	}

	void SpringArm::SetLength(float length)
	{
		springLength = length;
		JPH::Mat44 rotationMatrix = JPH::Mat44::sRotation(JPH::Quat::sEulerAngles(eulerRotation));
		springVector = rotationMatrix * JPH::Vec3::sAxisZ() * springLength;
	}

	void SpringArm::Update(PhysicsServer* physicsServer)
	{
		using namespace JPH;

		// DO raycast and update resultposition
		PhysicsSystem* physicsSystem = physicsServer->GetPhysicsSystem();
		AllHitCollisionCollector<RayCastBodyCollector> collector;
		Vec3 origin{};
		switch (originType)
		{
			case OriginType::Body:
			{
				origin = physicsSystem->GetBodyInterface().GetPosition(originBody);
				break;
			}

			case OriginType::Character:
			{
				origin = physicsServer->GetCharacterControllerPosition(originCharacterId);
			}
		}

		RayCast springRayCast = RayCast(origin, springVector);
		physicsSystem->GetBroadPhaseQuery().CastRay(springRayCast, collector);
		
		Vec3 calculatedSpringDistance = collector.mHits.empty() ? springVector : springVector * collector.mHits[0].mFraction;
		resultPosition = origin + calculatedSpringDistance;
	}

	JPH::Vec3 SpringArm::GetResultPosition() const
	{
		return resultPosition;
	}
}
