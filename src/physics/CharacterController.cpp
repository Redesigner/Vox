#include "CharacterController.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "raylib.h"

#include "physics/ObjectLayerTypes.h"
#include "physics/PhysicsServer.h"

namespace Vox
{
	CharacterController::CharacterController(JPH::PhysicsSystem* physicsSystem)
	{
		using namespace JPH;

		const float characterRadius = 1.0f;
		CapsuleShapeSettings capsuleShapeSettings(3.0f, characterRadius);
		ShapeSettings::ShapeResult capsuleShapeResult = capsuleShapeSettings.Create();
		capsuleShape = capsuleShapeResult.Get();
		Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
		settings->mMaxSlopeAngle = DEG2RAD * 50.0f;
		settings->mMaxStrength = 5.0f;
		settings->mShape = capsuleShape;
		settings->mBackFaceMode = JPH::EBackFaceMode::IgnoreBackFaces;
		settings->mCharacterPadding = 0.01f;
		settings->mPenetrationRecoverySpeed = 1.0f;
		settings->mPredictiveContactDistance = 0.1f;
		settings->mSupportingVolume = Plane(Vec3::sAxisY(), -characterRadius); // Accept contacts that touch the lower sphere of the capsule
		character = new CharacterVirtual(settings, RVec3(2.0f, 15.0f, 0.0f), Quat::sIdentity(), 0, physicsSystem);
		// character->SetListener(this);
	}

	JPH::Vec3 CharacterController::GetPosition() const
	{
		return character->GetPosition();
	}

	void CharacterController::Update(float deltaTime, PhysicsServer* physicsServer)
	{

		JPH::PhysicsSystem* physicsSystem = physicsServer->GetPhysicsSystem();
		JPH::Vec3 currentVelocity = character->GetLinearVelocity();
		JPH::Vec3 gravity = physicsSystem->GetGravity();
		currentVelocity += gravity * deltaTime;
		character->SetLinearVelocity(currentVelocity);
		character->Update(deltaTime, physicsSystem->GetGravity(), physicsSystem->GetDefaultBroadPhaseLayerFilter(Physics::CollisionLayer::Dynamic), physicsSystem->GetDefaultLayerFilter(Physics::CollisionLayer::Dynamic), {}, {}, *physicsServer->GetAllocator());
	}
}