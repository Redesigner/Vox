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
		unsigned int currentIndex;

		// size_t emptyIndex
	public:
		explicit ObjectContainer(size_t size)
		{
			backingData = std::vector<T>();
			backingIds = std::vector<int>();
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

		template <class... Args>
		std::pair<size_t, int> Create(Args&&... args)
		{
			backingData.emplace_back(std::forward<Args>(args)...);
			backingIds.emplace_back(currentIndex);
			std::pair<size_t, int> resultPair = std::pair<size_t, int>(backingData.size() - 1, currentIndex++);
			return resultPair;
		}

		size_t size() const
		{
			return backingData.size();
		}

		typename std::vector<T>::iterator begin() { return backingData.begin(); }
		typename std::vector<T>::const_iterator begin() const { return backingData.cbegin(); }
		typename std::vector<T>::iterator end() { return backingData.end(); }
		typename std::vector<T>::const_iterator end() const { return backingData.cend(); }
	};
}