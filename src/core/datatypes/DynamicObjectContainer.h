#pragma once

#include <array>
#include <utility>
#include <vector>

#include "core/datatypes/ObjectContainer.h"

namespace Vox
{
	template <typename T>
	class DynamicObjectContainer : public ObjectContainer<T>
	{
	private:
		std::vector<std::pair<size_t, int>> dirtyInstances;

	public:
		DynamicObjectContainer()
			:ObjectContainer<T>()
		{ }

		DynamicObjectContainer(size_t size)
			:ObjectContainer<T>(size)
		{
			dirtyInstances = std::vector<std::pair<size_t, int>>();
		}

		void MarkDirty(size_t index, int id)
		{
			for (const std::pair<size_t, int>& dirtyInstance : dirtyInstances)
			{
				if (dirtyInstance.first == index)
				{
					return;
				}
			}

			dirtyInstances.emplace_back(std::pair<size_t, int>(index, id));
		}

		void ClearDirty()
		{
			dirtyInstances.clear();
		}

		const std::vector<std::pair<size_t, int>>& GetDiryIndices() const
		{
			return dirtyInstances;
		}
	};
}