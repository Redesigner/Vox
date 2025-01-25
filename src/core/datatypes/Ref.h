#pragma once

#include <utility>
#include <vector>

#include "core/datatypes/ObjectContainer.h"

namespace Vox
{
	template <typename T>
	struct Ref
	{
	// @TODO: What happens if our reference is invalidated?
	public:
		Ref()
			:container(nullptr), index(0), id(0)
		{
		}

		Ref(ObjectContainer<T>* container, std::pair<size_t, int> id)
			:container(container), index(id.first), id(id.second)
		{
		}

		T* operator->()
		{
			assert(container);
			return container->Get(index, id);
		}

		void MarkDirty()
		{
			assert(container);
			container->MarkDirty(index, id);
		}

	private:
		ObjectContainer<T>* container;
		size_t index;
		int id;
	};
}