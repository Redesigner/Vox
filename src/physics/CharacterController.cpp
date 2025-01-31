#include "CharacterController.h"

#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "physics/ObjectLayerTypes.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	CharacterController::CharacterController(float inRadius, float inHalfHeight, JPH::PhysicsSystem* physicsSystem)
		:requestedVelocity(0.0f, 0.0f, 0.0f)
	{
		using namespace JPH;

		radius = inRadius;
		halfHeight = inHalfHeight;

		CapsuleShapeSettings capsuleShapeSettings(halfHeight, radius);
		ShapeSettings::ShapeResult capsuleShapeResult = capsuleShapeSettings.Create();
		capsuleShape = capsuleShapeResult.Get();
		JPH::Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
		settings->mMaxSlopeAngle =  glm::radians(50.0f);
		settings->mMaxStrength = 5.0f;
		settings->mShape = capsuleShape;
		settings->mBackFaceMode = JPH::EBackFaceMode::IgnoreBackFaces;
		settings->mCharacterPadding = 0.01f;
		settings->mPenetrationRecoverySpeed = 1.0f;
		settings->mPredictiveContactDistance = 0.1f;
		settings->mSupportingVolume = Plane(Vec3::sAxisY(), -radius); // Accept contacts that touch the lower sphere of the capsule
		character = new CharacterVirtual(settings, RVec3(2.0f, 15.0f, 0.0f), Quat::sIdentity(), 0, physicsSystem);
		// character->SetListener(this);
	}

	JPH::Vec3 CharacterController::GetPosition() const
	{
		return character->GetPosition();
	}

	JPH::Vec3 CharacterController::GetVelocity() const
	{
		return character->GetLinearVelocity();
	}

	JPH::Quat CharacterController::GetRotation() const
	{
		return character->GetRotation();
	}

	void CharacterController::Update(float deltaTime, PhysicsServer* physicsServer)
	{

		JPH::PhysicsSystem* physicsSystem = physicsServer->GetPhysicsSystem();
		JPH::Vec3 currentVelocity = character->GetLinearVelocity();
		currentVelocity.SetX(requestedVelocity.GetX());
		currentVelocity.SetZ(requestedVelocity.GetZ());
		if (character->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround)
		{
			if (currentVelocity.GetY() <= 0.0f)
			{
				currentVelocity.SetY(0.0f);
			}
		}
		else
		{
			JPH::Vec3 gravity = physicsSystem->GetGravity();
			currentVelocity += gravity * deltaTime;
		}

		character->SetLinearVelocity(currentVelocity);
		character->Update(deltaTime, physicsSystem->GetGravity(), physicsSystem->GetDefaultBroadPhaseLayerFilter(Physics::CollisionLayer::Dynamic), physicsSystem->GetDefaultLayerFilter(Physics::CollisionLayer::Dynamic), {}, {}, *physicsServer->GetAllocator());
	}

	float CharacterController::GetRadius() const
	{
		return radius;
	}

	float CharacterController::GetHalfHeight() const
	{
		return halfHeight;
	}
}