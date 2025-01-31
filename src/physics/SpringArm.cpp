#include "SpringArm.h"

#include <cmath>

#include <fmt/format.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "core/services/InputService.h"
#include "core/services/ServiceLocator.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	SpringArm::SpringArm()
	{
		origin = JPH::Vec3::sZero();
		originType = OriginType::CharacterOrigin;
		eulerRotation = JPH::Vec3::sZero();
		originBody = JPH::BodyID();
		resultPosition = JPH::Vec3::sZero();
		springLength = 0.0f;
		springOffset = JPH::Vec3::sZero();

		ServiceLocator::GetInputService()->RegisterMouseMotionCallback([this](int xMotion, int yMotion)
			{
				float xRot = eulerRotation.GetX();
				xRot += static_cast<float>(yMotion) / 500.0f;
				xRot = std::clamp(xRot, -1.5f, 1.5f);
				eulerRotation.SetX(xRot);

				float yRot = eulerRotation.GetY();
				yRot += static_cast<float>(xMotion) / 500.0f;
				eulerRotation.SetY(yRot);
			});
	}

	void SpringArm::SetOrigin(JPH::BodyID bodyId)
	{
		originType = OriginType::BodyOrigin;
		originBody = bodyId;
	}

	void SpringArm::SetOrigin(Ref<CharacterController> originCharacterIn)
	{
		originType = OriginType::CharacterOrigin;
		originCharacter = originCharacterIn;
	}

	JPH::Vec3 SpringArm::GetOrigin() const
	{
		return origin;
	}

	void SpringArm::SetOffset(JPH::Vec3 offset)
	{
		springOffset = offset;
	}

	void SpringArm::SetEulerRotation(JPH::Vec3 rotation)
	{
		eulerRotation = rotation;
	}

	JPH::Vec3 SpringArm::GetEulerRotation() const
	{
		return eulerRotation;
	}

	void SpringArm::SetLength(float length)
	{
		springLength = length;
	}

	void SpringArm::Update(PhysicsServer* physicsServer)
	{
		using namespace JPH;

		// DO raycast and update resultposition
		PhysicsSystem* physicsSystem = physicsServer->GetPhysicsSystem();
		Vec3 originRotation{};
		switch (originType)
		{
			case OriginType::BodyOrigin:
			{
				origin = physicsSystem->GetBodyInterface().GetPosition(originBody);
				originRotation = physicsSystem->GetBodyInterface().GetRotation(originBody).GetEulerAngles();
				break;
			}

			case OriginType::CharacterOrigin:
			{
				origin = originCharacter->GetPosition();
				originRotation = originCharacter->GetRotation().GetEulerAngles();
			}
		}
		origin += springOffset;

		Mat44 rotationMatrix = Mat44::sRotation(Quat::sEulerAngles(eulerRotation));
		Vec3 springVector = rotationMatrix * Vec3::sAxisZ() * springLength;

		RRayCast springRayCast = RRayCast(origin, springVector);
		RayCastResult springRayCastResult = RayCastResult();
		physicsSystem->GetNarrowPhaseQuery().CastRay(springRayCast, springRayCastResult);
		
		Vec3 calculatedSpringDistance = springVector * springRayCastResult.mFraction;
		resultPosition = origin + calculatedSpringDistance;
		//fmt::print("Raycast origin: {}, {}, {} Result: {}, {}, {}\n", origin.GetX(), origin.GetY(), origin.GetZ(), resultPosition.GetX(), resultPosition.GetY(), resultPosition.GetZ());
		//fmt::print("Raycast length: {}\n", calculatedSpringDistance.Length());
	}

	JPH::Vec3 SpringArm::GetResultPosition() const
	{
		return resultPosition;
	}
}
