#pragma once

#include <utility>

#include "Ref.h"
#include "core/datatypes/ObjectContainer.h"

namespace Vox
{
	template <typename T>
	struct WeakRef
	{
		WeakRef()
			:container(nullptr), index(0), id(0)
		{
		}

		explicit WeakRef(const Ref<T>& reference)
			:container(reference.container), index(reference.index), id(reference.id)
		{
		}
		
		T* operator->()
		{
			// assert(container);
			return container->Get(index, id);
		}

		const T* operator->() const
		{
			// assert(container);
			return container->Get(index, id);
		}

		explicit operator bool() const
		{
			return container != nullptr && container->Get(index, id) != nullptr;
		}

	private:
		ObjectContainer<T>* container;
		size_t index;
		int id;
	};
}