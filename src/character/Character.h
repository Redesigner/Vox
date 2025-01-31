#pragma once

#include "core/datatypes/Ref.h"

namespace Vox
{
	class CharacterController;
	struct MeshInstance;

	class Character
	{
	public:
		Character(Ref<CharacterController> characterController, Ref<MeshInstance> meshInstance);

		void Update();

	private:
		Ref<CharacterController> characterController;
		Ref<MeshInstance> meshInstance;
	};
}