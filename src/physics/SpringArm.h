#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Vox
{
	class CharacterController;
	class PhysicsServer;

	enum OriginType : char
	{
		Body,
		Character
	};

	class SpringArm
	{
	public:
		void SetOrigin(JPH::BodyID bodyId);
		void SetOrigin(unsigned int characterControllerId);

		void SetEulerRotation(JPH::Vec3 rotation);
		JPH::Vec3 GetEulerRotation() const;

		void SetLength(float length);

		//@TODO: restructure so that we don't need to inject the server like this
		void Update(PhysicsServer* physicsServer);

		JPH::Vec3 GetResultPosition() const;

	private:
		JPH::BodyID originBody;
		unsigned int originCharacterId;

		JPH::Vec3 springVector;

		JPH::Vec3 eulerRotation;

		float springLength;

		JPH::Vec3 resultPosition;

		OriginType originType;
	};
}