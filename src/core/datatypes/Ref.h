#pragma once

#include <utility>
#include <vector>

#include "core/datatypes/ObjectContainer.h"

namespace Vox
{
	template <typename T>
	struct Ref
	{
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
			return container != nullptr;
		}

	private:
		ObjectContainer<T>* container;
		size_t index;
		int id;
	};
}