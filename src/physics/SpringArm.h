#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "core/datatypes/Ref.h"

namespace Vox
{
	class CharacterController;
	class PhysicsServer;

	enum OriginType : char
	{
		BodyOrigin,
		CharacterOrigin
	};

	class SpringArm
	{
	public:
        [[deprecated]]
		SpringArm();

		void SetOrigin(JPH::BodyID bodyId);
		void SetOrigin(Ref<CharacterController> originCharacterIn);
		JPH::Vec3 GetOrigin() const;

		void SetOffset(JPH::Vec3 offset);

		void SetEulerRotation(JPH::Vec3 rotation);
		JPH::Vec3 GetEulerRotation() const;

		void SetLength(float length);

		//@TODO: restructure so that we don't need to inject the server like this
		void Update(PhysicsServer* physicsServer);

		JPH::Vec3 GetResultPosition() const;

	private:
		JPH::BodyID originBody;
		Ref<CharacterController> originCharacter;

		JPH::Vec3 springOffset;

		JPH::Vec3 eulerRotation;

		JPH::Vec3 origin;

		float springLength;

		JPH::Vec3 resultPosition;

		OriginType originType;
	};
}