//
// Created by steph on 6/19/2025.
//

#pragma once
#include <cassert>
#include <memory>
#include <variant>
#include <vector>

#include "core/logging/Logging.h"
#include "core/math/Math.h"

namespace Vox
{

    template <typename T>
    class TypedNode
    {
        // relevant physics information here
        enum class State : char
        {
            Full,
            Empty,
            Partial
        };

            using NodeVariant = std::variant<std::unique_ptr<TypedNode>, std::unique_ptr<T>>;

        public:
            explicit TypedNode(unsigned int size);
            TypedNode(unsigned int size, const T& data);
            ~TypedNode();

            TypedNode(TypedNode&& other) noexcept;
            TypedNode& operator = (TypedNode&& other) noexcept;


            [[nodiscard]] unsigned short GetSize() const;

            [[nodiscard]] T* GetData(int x, int y, int z) const;

            void SetData(int x, int y, int z, const T& data);

            [[nodiscard]] std::vector<char> GetPacked() const;

            static std::shared_ptr<TypedNode> FromPacked(const std::vector<char>& data);

        private:
            void AccumulatePacked(std::vector<char>& data) const;

            void Unpack(const std::vector<char>& data, size_t& currentIndex);

            static int GetIndex(int x, int y, int z);

            [[nodiscard]] NodeVariant& GetAccessor(int x, int y, int z);
            [[nodiscard]] const NodeVariant& GetAccessor(int x, int y, int z) const;

            void CollapseSubnodes(const T& data);

            std::array<NodeVariant, 8> subNodes;
            State state;
            unsigned short size;
    };

    template <typename T>
    TypedNode<T>::TypedNode(const unsigned int size)
        :size(size)
    {
        assert(IsPowerOfTwo(size));
        state = State::Empty;
    }

    template <typename T>
    TypedNode<T>::TypedNode(unsigned int size, const T& data)
        :TypedNode(size)
    {
        subNodes[0] = std::make_unique<T>(data);
        state = State::Full;
    }

    template <typename T>
    TypedNode<T>::~TypedNode()
    = default;

    template <typename T>
    TypedNode<T>::TypedNode(TypedNode&& other) noexcept
    {
        state = other.state;
        size = other.size;
        subNodes = std::move(other.subNodes);
    }

    template <typename T>
    TypedNode<T>& TypedNode<T>::operator=(TypedNode&& other) noexcept
    {
        state = other.state;
        size = other.size;
        subNodes = std::move(other.subNodes);
        return *this;
    }

    template <typename T>
    unsigned short TypedNode<T>::GetSize() const
    {
        return size;
    }

    template <typename T>
    T* TypedNode<T>::GetData(const int x, const int y, const int z) const
    {
        const int halfSize = size / 2;
        assert(x >= -halfSize && x < halfSize && y >= -halfSize && y < halfSize && z >= -halfSize && z < halfSize);

        switch (state)
        {
        case State::Empty:
            return nullptr;

        case State::Full:
            return std::get<std::unique_ptr<T>>(subNodes[0]).get();

        case State::Partial:
        {
            // Access the elements directly if we're at the smallest size
            if (size == 2)
            {
                return std::get<std::unique_ptr<T>>(GetAccessor(x, y, z)).get();
            }

            const int half = size / 4;
            const int subX = x + (x < 0 ? half : -half);
            const int subY = y + (y < 0 ? half : -half);
            const int subZ = z + (z < 0 ? half : -half);
            return std::get<std::unique_ptr<TypedNode>>(GetAccessor(x, y, z))->GetVoxel(subX, subY, subZ);
        }
        }

        return nullptr;
    }

    template <typename T>
    void TypedNode<T>::SetData(const int x, const int y, const int z, const T& data)
	{
		const int halfSize = size / 2;
	    assert(x >= -halfSize && x < halfSize && y >= -halfSize && y < halfSize && z >= -halfSize && z < halfSize);

		if (size == 2)
		{
			switch (state)
			{
			case State::Empty:
			{
				// We don't have any nodes so allocate new voxels
                // ReSharper disable once CppUseStructuredBinding -- Can't use this with unions
                for (auto& subNode : subNodes)
				{
				    subNode = std::make_unique<T>();
				}

			    GetAccessor(x, y, z) = std::make_unique<T>(data);
				state = State::Partial;
				return;
			}

			case State::Full:
			{
			    const auto currentData = *std::get<std::unique_ptr<T>>(subNodes[0]);
                if (currentData == data)
				{
					return;
				}

			    for (auto& node : subNodes)
			    {
			        node = std::make_unique<T>(currentData);
			    }

				GetAccessor(x, y, z) = std::make_unique<T>(data);
				state = State::Partial;
				return;
			}

			case State::Partial:
			{
				// Copy our new voxel into the relevant location
			    GetAccessor(x, y, z) = std::make_unique<T>(data);

				// Check if all the voxels are the same
				bool allSame = true;
				for (const auto& subNode : subNodes)
				{
                    if (const T nodeData = *std::get<std::unique_ptr<T>>(subNode); nodeData != data)
					{
						allSame = false;
						break;
					}
				}

				if (allSame)
				{
				    if (data)
				    {
				        state = State::Full;
				        for (auto& subNode : subNodes)
				        {
				            std::get<std::unique_ptr<T>>(subNode).reset();
				        }
				        subNodes[0] = std::make_unique<T>(data);
				    }
				    else
				    {
				        state = State::Empty;
				        for (auto& subNode : subNodes)
				        {
				            std::get<std::unique_ptr<T>>(subNode).reset();
				        }
				    }
				}
				return;
			}
			}
		}

		switch (state)
		{
		case State::Empty:
		{
			// Allocate subnodes
			const int half = size / 4;
			for (auto& subNode : subNodes)
			{
				subNode = std::make_unique<TypedNode>(size / 2);
			}
			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			std::get<std::unique_ptr<TypedNode>>(GetAccessor(x, y, z))->SetData(subX, subY, subZ, data);
			state = State::Partial;
			break;
		}

		case State::Full:
		{
			// The same as empty, but delete our "Full" node representation
			const T& fullVoxel = *std::get<std::unique_ptr<T>>(subNodes[0]);

			// This wouldn't change the node -- it should still be full
			if (fullVoxel == data)
			{
				return;
			}

			const int half = size / 4;

			for (auto& subNode : subNodes)
			{
				subNode = std::make_unique<TypedNode>(size / 2, fullVoxel);
			}

			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			std::get<std::unique_ptr<TypedNode>>(GetAccessor(x, y, z))->SetData(subX, subY, subZ, data);
			state = State::Partial;
			break;
		}

		case State::Partial:
		{
			const int half = size / 4;

			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			const auto& assignedNode = std::get<std::unique_ptr<TypedNode>>(GetAccessor(x, y, z));
			assignedNode->SetData(subX, subY, subZ, data);

			// check if the recently assigned node has been collapsed by our assignment
			if (assignedNode->state == State::Partial)
			{
				return;
			}

			for (const auto& subNode : subNodes)
			{
			    auto& node = std::get<std::unique_ptr<TypedNode>>(subNode);
			    if (node->state != State::Full)
				{
					return;
				}

			    auto& nodeFullVoxel = *std::get<std::unique_ptr<T>>(node->subNodes[0]);
			    if (nodeFullVoxel != data)
			    {
			        return;
			    }
			}
			// All nodes are full, and match the voxel we just inserted, collapse
			CollapseSubnodes(data);
			break;
		}
		}
	}

    template <typename T>
    std::vector<char> TypedNode<T>::GetPacked() const
    {
        std::vector<char> result;
        result.insert(result.begin(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(short));

        AccumulatePacked(result);

        return result;
    }

    template <typename T>
    std::shared_ptr<TypedNode<T>> TypedNode<T>::FromPacked(const std::vector<char>& data)
    {
        unsigned short treeSize = *reinterpret_cast<const unsigned short*>(&data[0]);

        if (treeSize < 2 || (treeSize & (treeSize - 1)) != 0)
        {
            VoxLog(Error, Game, "Unpacking octree failed: size constant is not a power of 2.");
            return nullptr;
        }

        auto root = std::make_shared<TypedNode>(treeSize);
        size_t currentIndex = sizeof(unsigned short);
        root->Unpack(data, currentIndex);
        return root;
    }

    template <typename T>
    void TypedNode<T>::AccumulatePacked(std::vector<char>& data) const
    {
        if (size == 2)
        {
            switch (state)
            {
            case State::Empty:
            {
                data.emplace_back('E');
                return;
            }
            case State::Full:
            {
                data.emplace_back('F');
                const auto& voxel = std::get<std::unique_ptr<T>>(subNodes[0]);
                data.insert(data.end(),
                    reinterpret_cast<const char*>(voxel.get()),
                    reinterpret_cast<const char*>(voxel.get()) + sizeof(T));
                return;
            }
            case State::Partial:
            {
                data.emplace_back('P');
                for (const auto& subNode : subNodes)
                {
                    const auto& voxel = std::get<std::unique_ptr<T>>(subNode);
                    data.insert(data.end(),
                    reinterpret_cast<const char*>(voxel.get()),
                    reinterpret_cast<const char*>(voxel.get()) + sizeof(T));
                }
                return;
            }
            }
        }

        switch (state)
        {
        case State::Empty:
        {
            data.emplace_back('E');
            return;
        }
        case State::Full:
        {
            data.emplace_back('F');
            const auto& voxel = std::get<std::unique_ptr<T>>(subNodes[0]);
            data.insert(data.end(),
                reinterpret_cast<const char*>(voxel.get()),
                reinterpret_cast<const char*>(voxel.get()) + sizeof(T));
            return;
        }
        case State::Partial:
        {
            data.emplace_back('P');
            for (const auto& subNode : subNodes)
            {
                std::get<std::unique_ptr<TypedNode>>(subNode)->AccumulatePacked(data);
            }
            return;
        }
        }
    }

    template <typename T>
    void TypedNode<T>::Unpack(const std::vector<char>& data, size_t& currentIndex)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const char currentStateChar = data[currentIndex];

        if (currentStateChar == 'E')
        {
            state = State::Empty;
        }
        else if (currentStateChar == 'F')
        {
            state = State::Full;
        }
        else if (currentStateChar == 'P')
        {
            state = State::Partial;
        }
        else
        {
            VoxLog(Error, Game, "Failure unpacking octree. A node was malformed.");
            return;
        }
        currentIndex++;

        if (size == 2)
        {
            switch (state)
            {
            case State::Empty:
            {
                std::get<std::unique_ptr<TypedNode>>(subNodes[0]) = nullptr;
                return;
            }

            case State::Full:
            {
                subNodes[0] = std::make_unique<T>(*reinterpret_cast<const T*>(&data[currentIndex]));
                currentIndex += sizeof(T);
                return;
            }

            case State::Partial:
            {
                for (auto& subNode: subNodes)
                {
                    subNode = std::make_unique<T>(*reinterpret_cast<const T*>(&data[currentIndex]));
                    currentIndex += sizeof(T);
                }
                return;
            }
            }
        }

        switch (state)
        {
        case State::Empty:
        {
            std::get<std::unique_ptr<TypedNode>>(subNodes[0]) = nullptr;
            return;
        }

        case State::Full:
        {
            subNodes[0]= std::make_unique<T>(*reinterpret_cast<const T*>(&data[currentIndex]));
            currentIndex += sizeof(T);
            return;
        }

        case State::Partial:
        {
            for (auto& subNode : subNodes)
            {
                subNode = std::make_unique<TypedNode>(size / 2);
                std::get<std::unique_ptr<TypedNode>>(subNode)->Unpack(data, currentIndex);
            }
        }
        }
    }

    template <typename T>
    int TypedNode<T>::GetIndex(int x, int y, int z)
    {
        if (x < 0)
        {
            if (y < 0)
            {
                if (z < 0)
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                if (z < 0)
                {
                    return 2;
                }
                else
                {
                    return 3;
                }
            }
        }
        else
        {
            if (y < 0)
            {
                if (z < 0)
                {
                    return 4;
                }
                else
                {
                    return 5;
                }
            }
            else
            {
                if (z < 0)
                {
                    return 6;
                }
                else
                {
                    return 7;
                }
            }
        }
    }

    template <typename T>
    typename TypedNode<T>::NodeVariant& TypedNode<T>::GetAccessor(int x, int y, int z)
    {
		return subNodes[GetIndex(x, y, z)];
    }

    template <typename T>
    const typename TypedNode<T>::NodeVariant& TypedNode<T>::GetAccessor(int x, int y, int z) const
    {
		return subNodes[GetIndex(x, y, z)];
    }

    template <typename T>
    void TypedNode<T>::CollapseSubnodes(const T& data)
    {
        for (int i = 1; i < 8; ++i)
        {
            std::get<std::unique_ptr<TypedNode>>(subNodes[i]).reset();
        }

        subNodes[0] = std::make_unique<T>(data);
        state = State::Full;
    }

} // Vox
