#pragma once

#include <cassert>
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
		std::vector<unsigned int> backingRefCount;
		unsigned int currentIndex;

		// size_t emptyIndex
	public:
		explicit ObjectContainer(size_t size)
		{
			backingData = std::vector<T>();
			backingIds = std::vector<int>();
			backingRefCount = std::vector<unsigned int>();
			currentIndex = 0;
		}

		ObjectContainer()
			:ObjectContainer(0)
		{ }

		T* Get(size_t index, const int id)
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
			backingRefCount.emplace_back(0);
			const auto resultPair = std::pair<size_t, int>(backingData.size() - 1, currentIndex++);
			return resultPair;
		}

		[[nodiscard]] size_t size() const
		{
			return backingData.size();
		}

		void IncrementRefCount(const std::pair<size_t, int> refPair)
		{
			assert(refPair.first < backingIds.size());
			if (backingIds[refPair.first] == refPair.second)
			{
				backingRefCount[refPair.first]++;
			}
		}

		void DecrementRefCount(const std::pair<size_t, int> refPair)
		{
			if (refPair.first >= backingIds.size())
			{
				return;
			}

			if (backingIds[refPair.first] == refPair.second)
			{
				assert(backingRefCount[refPair.first] > 0);
				if (--backingRefCount[refPair.first] == 0)
				{
					backingData.erase(backingData.begin() + refPair.first);
					backingIds.erase(backingIds.begin() + refPair.first);
					backingRefCount.erase(backingRefCount.begin() + refPair.first);
				}
			}
		}
		
		typename std::vector<T>::iterator begin() { return backingData.begin(); }
		typename std::vector<T>::const_iterator begin() const { return backingData.cbegin(); }
		typename std::vector<T>::iterator end() { return backingData.end(); }
		typename std::vector<T>::const_iterator end() const { return backingData.cend(); }
	};
}