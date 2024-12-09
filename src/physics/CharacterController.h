#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace Vox
{
	class CharacterController
	{
		
	private:
		JPH::Ref<JPH::CharacterVirtual> character;
	};
}