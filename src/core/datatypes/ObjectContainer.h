#pragma once

#include <array>
#include <utility>
#include <vector>

namespace Vox
{
	struct IndexId
	{
		size_t index;
		int id;
	};

	template <typename T>
	class ObjectContainer
	{
	private:
		std::vector<T> backingData;
		std::vector<int> backingIds;
		std::vector<std::pair<size_t, int>> dirtyInstances;
		unsigned int currentIndex;

		// size_t emptyIndex
	public:
		ObjectContainer(size_t size)
		{
			backingData = std::vector<T>();
			backingIds = std::vector<int>();
			dirtyInstances = std::vector<std::pair<size_t, int>>();
			currentIndex = 0;
		}

		ObjectContainer()
			:ObjectContainer(0)
		{ }

		T* Get(size_t index, int id)
		{
			if (backingIds.at(index) != id)
			{
				return nullptr;
			}
			return &backingData.at(index);
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

		template <class... Args>
		std::pair<size_t, int> Create(Args&&... args)
		{
			backingData.emplace_back(std::forward<Args>(args)...);
			backingIds.emplace_back(currentIndex);
			std::pair<size_t, int> resultPair = std::pair<size_t, int>(backingData.size() - 1, currentIndex++);
			return resultPair;
		}

		void ClearDirty()
		{
			dirtyInstances.clear();
		}

		std::vector<T>::iterator begin() { return backingData.begin(); }
		std::vector<T>::iterator end() { return backingData.end(); }

		const std::vector<std::pair<size_t, int>>& GetDiryIndices() const
		{
			return dirtyInstances;
		}
	};
}