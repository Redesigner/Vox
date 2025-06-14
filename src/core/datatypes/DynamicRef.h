#pragma once

#include <utility>
#include <vector>

#include "core/datatypes/DynamicObjectContainer.h"

namespace Vox
{
	template <typename T>
	struct DynamicRef
	{
	public:
		DynamicRef()
			:container(nullptr), index(0), id(0)
		{
		}

		DynamicRef(DynamicObjectContainer<T>* container, const std::pair<size_t, int> id)
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
		DynamicObjectContainer<T>* container;
		size_t index;
		int id;
	};
}