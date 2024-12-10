#include "CharacterController.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "raylib.h"

#include "physics/PhysicsServer.h"

namespace Vox
{
	CharacterController::CharacterController(JPH::PhysicsSystem* physicsSystem)
	{
		using namespace JPH;

		const float characterRadius = 1.0f;
		CapsuleShapeSettings capsuleShapeSettings(4.0f, characterRadius);
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
		character = new CharacterVirtual(settings, RVec3::sZero(), Quat::sIdentity(), 0, physicsSystem);
		// character->SetListener(this);
	}
}