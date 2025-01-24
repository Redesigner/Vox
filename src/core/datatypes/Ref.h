#pragma once

#include <vector>

namespace Vox
{
	template <typename T>
	struct Ref
	{
	// @TODO: What happens if our reference is invalidated?
	public:
		Ref()
			:container(nullptr), index(0)
		{
		}

		Ref(std::vector<T>* container, size_t index)
			:container(container), index(index)
		{
		}

		T* operator->()
		{
			assert(container);
			return &container->at(index);
		}

		T& operator*()
		{
			assert(container);
			return container->at(index);
		}

		const T& operator*() const
		{
			assert(container);
			return container->at(index);
		}

	private:
		std::vector<T>* container;
		size_t index;
	};
}