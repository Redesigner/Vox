// ReSharper disable CppUseStructuredBinding
// Don't use bindings with unions!!!
#include "CollisionOctree.h"

#include <cassert>
#include <cstring>
#include <cmath>
#include <stdexcept>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#include "core/logging/Logging.h"

namespace Octree
{
	CollisionNode::CollisionNode(const unsigned int size)
		:size(size)
	{
		assert((size & (size - 1)) == 0);
		state = State::Empty;
	}

	CollisionNode::CollisionNode(const unsigned int size, const PhysicsVoxel& voxel)
		:CollisionNode(size)
	{
	    // Size must be power of 2
		assert((size & (size - 1)) == 0);

		subNodes[0] = std::make_unique<PhysicsVoxel>(voxel);
		state = State::Full;
	}

	CollisionNode::~CollisionNode()
	= default;

    CollisionNode::CollisionNode(CollisionNode&& other) noexcept
    {
        state = other.state;
        size = other.size;
        subNodes = std::move(other.subNodes);
    }

    CollisionNode& CollisionNode::operator=(CollisionNode&& other) noexcept
    {
        state = other.state;
        size = other.size;
        subNodes = std::move(other.subNodes);
        return *this;
    }

    unsigned short CollisionNode::GetSize() const
	{
		return size;
	}

	PhysicsVoxel* CollisionNode::GetVoxel(const int x, const int y, const int z) const // NOLINT(*-no-recursion)
    {
		const int halfSize = size / 2;
	    assert(x >= -halfSize && x < halfSize && y >= -halfSize && y < halfSize && z >= -halfSize && z < halfSize);

		switch (state)
		{
		case State::Empty:
			return nullptr;

		case State::Full:
			return std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]).get();

		case State::Partial:
		{
			// Access the elements directly if we're at the smallest size
			if (size == 2)
			{
				return std::get<std::unique_ptr<PhysicsVoxel>>(GetAccessor(x, y, z)).get();
			}

			const int half = size / 4;
			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);
			return std::get<std::unique_ptr<CollisionNode>>(GetAccessor(x, y, z))->GetVoxel(subX, subY, subZ);
		}
		}

		return nullptr;
	}

	void CollisionNode::SetVoxel(const int x, const int y, const int z, const PhysicsVoxel& voxel) // NOLINT(*-no-recursion)
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
				    subNode = std::make_unique<PhysicsVoxel>();
				}

			    GetAccessor(x, y, z) = std::make_unique<PhysicsVoxel>(voxel);
				state = State::Partial;
				return;
			}

			case State::Full:
			{
			    const auto currentVoxel = *std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]);
                if (currentVoxel == voxel)
				{
					return;
				}

			    for (auto& node : subNodes)
			    {
			        node = std::make_unique<PhysicsVoxel>(currentVoxel);
			    }

				GetAccessor(x, y, z) = std::make_unique<PhysicsVoxel>(voxel);
				state = State::Partial;
				return;
			}

			case State::Partial:
			{
				// Copy our new voxel into the relevant location
			    GetAccessor(x, y, z) = std::make_unique<PhysicsVoxel>(voxel);

				// Check if all the voxels are the same
				bool allSame = true;
				for (const auto& subNode : subNodes)
				{
                    if (const PhysicsVoxel nodeVoxel = *std::get<std::unique_ptr<PhysicsVoxel>>(subNode); nodeVoxel != voxel)
					{
						allSame = false;
						break;
					}
				}

				if (allSame)
				{
				    if (voxel.solid)
				    {
				        state = State::Full;
				        //subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel(voxel));
				        for (auto& subNode : subNodes)
				        {
				            std::get<std::unique_ptr<PhysicsVoxel>>(subNode).reset();
				        }
				        subNodes[0] = std::make_unique<PhysicsVoxel>(voxel);
				    }
				    else
				    {
				        state = State::Empty;
				        for (auto& subNode : subNodes)
				        {
				            std::get<std::unique_ptr<PhysicsVoxel>>(subNode).reset();
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
				subNode = std::make_unique<CollisionNode>(size / 2);
			}
			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			std::get<std::unique_ptr<CollisionNode>>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = State::Partial;
			break;
		}

		case State::Full:
		{
			// The same as empty, but delete our "Full" node representation
			const PhysicsVoxel& fullVoxel = *std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]);

			// This wouldn't change the node -- it should still be full
			if (fullVoxel == voxel)
			{
				return;
			}

			const int half = size / 4;

			for (auto& subNode : subNodes)
			{
				subNode = std::make_unique<CollisionNode>(size / 2, fullVoxel);
			}

			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			std::get<std::unique_ptr<CollisionNode>>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = State::Partial;
			break;
		}

		case State::Partial:
		{
			const int half = size / 4;

			const int subX = x + (x < 0 ? half : -half);
			const int subY = y + (y < 0 ? half : -half);
			const int subZ = z + (z < 0 ? half : -half);

			const auto& assignedNode = std::get<std::unique_ptr<CollisionNode>>(GetAccessor(x, y, z));
			assignedNode->SetVoxel(subX, subY, subZ, voxel);

			// check if the recently assigned node has been collapsed by our assignment
			if (assignedNode->state == State::Partial)
			{
				return;
			}

			for (const auto& subNode : subNodes)
			{
			    auto& collisionNode = std::get<std::unique_ptr<CollisionNode>>(subNode);
			    if (collisionNode->state != State::Full)
				{
					return;
				}

			    auto& nodeFullVoxel = *std::get<std::unique_ptr<PhysicsVoxel>>(collisionNode->subNodes[0]);
			    if (nodeFullVoxel != voxel)
			    {
			        return;
			    }
			}
			// All nodes are full, and match the voxel we just inserted, collapse
			CollapseSubnodes(voxel);
			break;
		}
		}
	}

	JPH::Ref<JPH::StaticCompoundShapeSettings> CollisionNode::MakeCompoundShape() const
	{
		using namespace JPH;
		Ref shapeSettings = new StaticCompoundShapeSettings;
        const std::vector<Cube> cubes = GetCubes();
        VoxLog(Display, Physics, "Created voxel body with '{}' cubes", cubes.size());
        for (const Cube& cube : cubes)
		{
			const float cubeHalfExtent = static_cast<float>(cube.size) / 2.0f;
			if (cube.size == 1)
			{
				shapeSettings->AddShape({
				    static_cast<float>(cube.x) + cubeHalfExtent,
				    static_cast<float>(cube.y) + cubeHalfExtent,
				    static_cast<float>(cube.z) + cubeHalfExtent},
				    Quat::sIdentity(), new BoxShape(Vec3(cubeHalfExtent, cubeHalfExtent, cubeHalfExtent)));
			}
			else
			{
				shapeSettings->AddShape({
				    static_cast<float>(cube.x),
				    static_cast<float>(cube.y),
				    static_cast<float>(cube.z)},
				    Quat::sIdentity(), new BoxShape(Vec3(cubeHalfExtent, cubeHalfExtent, cubeHalfExtent)));
			}
		}

		return shapeSettings;
	}

	std::vector<Cube> CollisionNode::GetCubes() const
	{
		std::vector<Cube> result;
		GenerateCubes(0, 0, 0, result);
		return result;
	}

	std::vector<char> CollisionNode::GetPacked() const
	{
		std::vector<char> result;
		result.insert(result.begin(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(short));

		AccumulatePacked(result);

		return result;
	}

	std::shared_ptr<CollisionNode> CollisionNode::FromPacked(const std::vector<char>& data)
	{
		unsigned short treeSize = *reinterpret_cast<const unsigned short*>(&data[0]);

		if (treeSize < 2 || (treeSize & (treeSize - 1)) != 0)
		{
			VoxLog(Error, Game, "Unpacking octree failed: size constant is not a power of 2.");
			return nullptr;
		}

		auto root = std::make_shared<CollisionNode>(treeSize);
		size_t currentIndex = sizeof(unsigned short);
		root->Unpack(data, currentIndex);
		return root;
	}

	void CollisionNode::GenerateCubes(int x, int y, int z, std::vector<Cube>& cubes) const // NOLINT(*-no-recursion)
	{
		switch (state)
		{
		case State::Empty:
		{
			return;
		}
		case State::Full:
		{
			cubes.emplace_back(x, y, z, size);
			return;
		}
		case State::Partial:
		{
			if (size == 2)
			{
                const auto* physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y - 1, z - 1, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[1]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y - 1, z, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[2]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y, z - 1, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[3]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y, z, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[4]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x, y - 1, z - 1, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[5]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x, y - 1, z, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[6]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x, y, z - 1, 1); }
				physicsVoxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[7]).get();
				if (physicsVoxel->solid) { cubes.emplace_back(x, y, z, 1); }
			}
			else
			{
				const unsigned int halfSize = size / 4;
				std::get<std::unique_ptr<CollisionNode>>(subNodes[0])->GenerateCubes(static_cast<int>(x - halfSize), static_cast<int>(y - halfSize), static_cast<int>(z - halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[1])->GenerateCubes(static_cast<int>(x - halfSize), static_cast<int>(y - halfSize), static_cast<int>(z + halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[2])->GenerateCubes(static_cast<int>(x - halfSize), static_cast<int>(y + halfSize), static_cast<int>(z - halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[3])->GenerateCubes(static_cast<int>(x - halfSize), static_cast<int>(y + halfSize), static_cast<int>(z + halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[4])->GenerateCubes(static_cast<int>(x + halfSize), static_cast<int>(y - halfSize), static_cast<int>(z - halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[5])->GenerateCubes(static_cast<int>(x + halfSize), static_cast<int>(y - halfSize), static_cast<int>(z + halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[6])->GenerateCubes(static_cast<int>(x + halfSize), static_cast<int>(y + halfSize), static_cast<int>(z - halfSize), cubes);
				std::get<std::unique_ptr<CollisionNode>>(subNodes[7])->GenerateCubes(static_cast<int>(x + halfSize), static_cast<int>(y + halfSize), static_cast<int>(z + halfSize), cubes);
			}
			return;
		}
		}
	}

	void CollisionNode::AccumulatePacked(std::vector<char>& data) const // NOLINT(*-no-recursion)
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
				const auto& voxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]);
				data.insert(data.end(),
				    reinterpret_cast<const char*>(voxel.get()),
				    reinterpret_cast<const char*>(voxel.get()) + sizeof(PhysicsVoxel));
				return;
			}
			case State::Partial:
			{
				data.emplace_back('P');
				for (const auto& subNode : subNodes)
				{
				    const auto& voxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNode);
					data.insert(data.end(),
					reinterpret_cast<const char*>(voxel.get()),
					reinterpret_cast<const char*>(voxel.get()) + sizeof(PhysicsVoxel));
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
			const auto& voxel = std::get<std::unique_ptr<PhysicsVoxel>>(subNodes[0]);
			data.insert(data.end(),
			    reinterpret_cast<const char*>(voxel.get()),
			    reinterpret_cast<const char*>(voxel.get()) + sizeof(PhysicsVoxel));
			return;
		}
		case State::Partial:
		{
			data.emplace_back('P');
			for (const auto& subNode : subNodes)
			{
				std::get<std::unique_ptr<CollisionNode>>(subNode)->AccumulatePacked(data);
			}
			return;
		}
		}
	}

	void CollisionNode::Unpack(const std::vector<char>& data, size_t& currentIndex) // NOLINT(*-no-recursion)
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
				std::get<std::unique_ptr<CollisionNode>>(subNodes[0]) = nullptr;
				return;
			}

			case State::Full:
			{
				subNodes[0] = std::make_unique<PhysicsVoxel>(*reinterpret_cast<const PhysicsVoxel*>(&data[currentIndex]));
				currentIndex += sizeof(PhysicsVoxel);
				return;
			}

			case State::Partial:
			{
				for (auto& subNode: subNodes)
				{
				    subNode = std::make_unique<PhysicsVoxel>(*reinterpret_cast<const PhysicsVoxel*>(&data[currentIndex]));
					currentIndex += sizeof(PhysicsVoxel);
				}
				return;
			}
			}
		}

		switch (state)
		{
		case State::Empty:
		{
			std::get<std::unique_ptr<CollisionNode>>(subNodes[0]) = nullptr;
			return;
		}

		case State::Full:
		{
		    subNodes[0]= std::make_unique<PhysicsVoxel>(*reinterpret_cast<const PhysicsVoxel*>(&data[currentIndex]));
			currentIndex += sizeof(PhysicsVoxel);
			return;
		}

		case State::Partial:
		{
			for (auto& subNode : subNodes)
			{
				subNode = std::make_unique<CollisionNode>(size / 2);
				std::get<std::unique_ptr<CollisionNode>>(subNode)->Unpack(data, currentIndex);
			}
		}
		}
	}

	int CollisionNode::GetIndex(const int x, const int y, const int z)
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

	CollisionNode::NodeVariant& CollisionNode::GetAccessor(const int x, const int y, const int z)
	{
		return subNodes[GetIndex(x, y, z)];
	}

    const CollisionNode::NodeVariant& CollisionNode::GetAccessor(const int x, const int y, const int z) const
    {
		return subNodes[GetIndex(x, y, z)];
    }

    void CollisionNode::CollapseSubnodes(const PhysicsVoxel& voxel)
	{
		for (int i = 1; i < 8; ++i)
		{
			std::get<std::unique_ptr<CollisionNode>>(subNodes[i]).reset();
		}

		subNodes[0] = std::make_unique<PhysicsVoxel>(voxel);
		state = State::Full;
	}

	PhysicsVoxel::PhysicsVoxel()
	= default;

	PhysicsVoxel::PhysicsVoxel(const bool solid)
		:solid(solid)
	{
	}

	bool PhysicsVoxel::operator==(const PhysicsVoxel& voxel) const
	{
		return solid == voxel.solid;
	}

    bool PhysicsVoxel::operator!=(const PhysicsVoxel& voxel) const
    {
        return (!solid) != (!voxel.solid);
    }

    Cube::Cube(const int x, const int y, const int z, const unsigned int size)
		:x(x), y(y), z(z), size(size)
	{
	}
}
