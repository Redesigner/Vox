#pragma once

#include <utility>

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
			container->IncrementRefCount(id);
		}

		~Ref()
		{
			if (container)
			{
				container->DecrementRefCount({ index, id });
			}
		}

		Ref(const Ref<typename T>& other)
			:container(other.container), index(other.index), id(other.id)
		{
			container->IncrementRefCount({index, id});
		}

		Ref(Ref&& other)
			:container(other.container), index(other.index), id(other.id)
		{
		}

		Ref& operator =(const Ref& other)
		{
			if (&other == this)
			{
				return *this;
			}

			if (container)
			{
				container->DecrementRefCount({index, id});
			}
			container = other.container;
			index = other.index;
			id = other.id;
			container->IncrementRefCount({index, id});
			return *this;
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