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
	CollisionNode::CollisionNode(unsigned int size)
		:size(size)
	{
		assert((size & (size - 1)) == 0);
		subNodes[0] = nullptr;
		state = Empty;
	}

	CollisionNode::CollisionNode(unsigned int size, PhysicsVoxel* voxel)
		:size(size)
	{
		assert((size & (size - 1)) == 0);
		// Allocate a voxel that we can copy our incoming voxel to
		subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
		std::memcpy(subNodes[0], voxel, sizeof(PhysicsVoxel));
		state = Full;
	}

	CollisionNode::~CollisionNode()
	{
		if (size == 2)
		{
			switch (state)
			{
			case Full:
			{
				delete reinterpret_cast<PhysicsVoxel*>(subNodes[0]);
				break;
			}
			case Partial:
			{
				for (int i = 0; i < 8; ++i)
				{
					delete reinterpret_cast<PhysicsVoxel*>(subNodes[i]);
				}
			}
			}
			return;
		}

		switch (state)
		{
		case Full:
		{
			delete reinterpret_cast<PhysicsVoxel*>(subNodes[0]);
			break;
		}

		case Partial:
		{
			for (int i = 0; i < 8; ++i)
			{
				delete reinterpret_cast<CollisionNode*>(subNodes[i]);
			}
		}
		}
	}

	unsigned short CollisionNode::GetSize() const
	{
		return size;
	}

	PhysicsVoxel* CollisionNode::GetVoxel(int x, int y, int z)
	{
		int halfSize = size / 2;
		if (x < -halfSize || x >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}
		if (y < -halfSize || y >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}
		if (z < -halfSize || z >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}

		switch (state)
		{
		case Empty:
			return nullptr;
		case Full:
			return reinterpret_cast<PhysicsVoxel*>(subNodes[0]);
		case Partial:
		{
			// Access the elements directly if we're at the smallest size
			if (size == 2)
			{
				return reinterpret_cast<PhysicsVoxel*>(GetAccessor(x, y, z));
			}

			int half = size / 4;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);
			return reinterpret_cast<CollisionNode*>(GetAccessor(x, y, z))->GetVoxel(subX, subY, subZ);
		}
		}
		return nullptr;
	}

	void CollisionNode::SetVoxel(int x, int y, int z, PhysicsVoxel* voxel)
	{
		int halfSize = size / 2;
		if (x < -halfSize || x >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}
		if (y < -halfSize || y >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}
		if (z < -halfSize || z >= halfSize)
		{
			throw std::out_of_range("Accessed voxel outside of range.");
		}

		if (size == 2)
		{
			switch (state)
			{
			case Empty:
			{
				// We don't have any nodes so allocate new voxels
				subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[1] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[2] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[3] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[4] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[5] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[6] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[7] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);

				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(PhysicsVoxel));
				state = Partial;
				return;
			}

			case Full:
			{
				PhysicsVoxel* currentVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[0]);
				if (*currentVoxel == *voxel)
				{
					return;
				}
				// We only have one voxel, at index 0
				delete currentVoxel;

				subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[1] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[2] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[3] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[4] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[5] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[6] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				subNodes[7] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);

				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(PhysicsVoxel));
				state = Partial;
				return;
			}

			case Partial:
			{
				// Copy our new voxel into the relevant location
				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(PhysicsVoxel));

				// Check if all the voxels are the same
				bool allSame = true;
				for (int i = 0; i < 8; ++i)
				{
					PhysicsVoxel* subVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[i]);
					if (*subVoxel != *voxel)
					{
						allSame = false;
						break;
					}
				}

				if (allSame)
				{
					state = Full;
					std::memcpy(subNodes[0], voxel, sizeof(PhysicsVoxel));
					for (int i = 1; i < 8; ++i)
					{
						delete reinterpret_cast<PhysicsVoxel*>(subNodes[i]);
					}
				}
				return;
			}
			}
		}

		switch (state)
		{
		case Empty:
		{
			// Dont set an empty voxel to the already empty node
			if (voxel == nullptr)
			{
				return;
			}

			// Allocate subnodes
			int half = size / 4;
			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new CollisionNode(size / 2);
			}
			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			reinterpret_cast<CollisionNode*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = Partial;
			break;
		}

		case Full:
		{
			// The same as empty, but delete our "Full" node representation
			PhysicsVoxel* fullVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[0]);

			// This wouldn't change the node -- it should still be full
			if (*fullVoxel == *voxel)
			{
				return;
			}

			int half = size / 4;

			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new CollisionNode(size / 2, fullVoxel);
			}
			delete fullVoxel;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			reinterpret_cast<CollisionNode*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = Partial;
			break;
		}

		case Partial:
		{
			int half = size / 4;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			CollisionNode* assignedNode = reinterpret_cast<CollisionNode*>(GetAccessor(x, y, z));
			assignedNode->SetVoxel(subX, subY, subZ, voxel);

			// check if the recently assigned node has been collapsed by our assignment
			if (assignedNode->state == Partial)
			{
				return;
			}

			for (int i = 0; i < 8; ++i)
			{
				CollisionNode* subNode = reinterpret_cast<CollisionNode*>(subNodes[i]);
				if (subNode->state != Full || *reinterpret_cast<PhysicsVoxel*>(subNode->subNodes[0]) != *voxel)
				{
					return;
				}
			}
			// All nodes are full, and match the voxel we just insterted, collapse
			CollapseSubnodes(voxel);
			break;
		}
		}
	}

	JPH::Ref<JPH::StaticCompoundShapeSettings> CollisionNode::MakeCompoundShape() const
	{
		using namespace JPH;
		Ref<StaticCompoundShapeSettings> shapeSettings = new StaticCompoundShapeSettings;
		std::vector<Cube> cubes = GetCubes();
		for (const Cube& cube : cubes)
		{
			float cubeHalfExtent = static_cast<float>(cube.size / 2.0f);
			if (cube.size == 1)
			{
				shapeSettings->AddShape(Vec3(cube.x + cubeHalfExtent, cube.y + cubeHalfExtent, cube.z + cubeHalfExtent), Quat::sIdentity(), new BoxShape(Vec3(cubeHalfExtent, cubeHalfExtent, cubeHalfExtent)));
			}
			else
			{
				shapeSettings->AddShape(Vec3(cube.x, cube.y, cube.z), Quat::sIdentity(), new BoxShape(Vec3(cubeHalfExtent, cubeHalfExtent, cubeHalfExtent)));
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

		std::shared_ptr<CollisionNode> root = std::make_shared<CollisionNode>(treeSize);
		size_t currentIndex = sizeof(unsigned short);
		root->Unpack(data, currentIndex);
		return root;
	}

	void CollisionNode::GenerateCubes(int x, int y, int z, std::vector<Cube>& cubes) const
	{
		switch (state)
		{
		case Empty:
		{
			return;
		}
		case Full:
		{
			cubes.emplace_back(x, y, z, size);
			return;
		}
		case Partial:
		{
			if (size == 2)
			{
				PhysicsVoxel* physicsVoxel;
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[0]);
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y - 1, z - 1, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[1]);
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y - 1, z, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[2]);
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y, z - 1, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[3]);
				if (physicsVoxel->solid) { cubes.emplace_back(x - 1, y, z, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[4]);
				if (physicsVoxel->solid) { cubes.emplace_back(x, y - 1, z - 1, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[5]);
				if (physicsVoxel->solid) { cubes.emplace_back(x, y - 1, z, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[6]);
				if (physicsVoxel->solid) { cubes.emplace_back(x, y, z - 1, 1); }
				physicsVoxel = reinterpret_cast<PhysicsVoxel*>(subNodes[7]);
				if (physicsVoxel->solid) { cubes.emplace_back(x, y, z, 1); }
			}
			else
			{
				unsigned int halfSize = size / 4;
				subNodes[0]->GenerateCubes(x - halfSize, y - halfSize, z - halfSize, cubes);
				subNodes[1]->GenerateCubes(x - halfSize, y - halfSize, z + halfSize, cubes);
				subNodes[2]->GenerateCubes(x - halfSize, y + halfSize, z - halfSize, cubes);
				subNodes[3]->GenerateCubes(x - halfSize, y + halfSize, z + halfSize, cubes);
				subNodes[4]->GenerateCubes(x + halfSize, y - halfSize, z - halfSize, cubes);
				subNodes[5]->GenerateCubes(x + halfSize, y - halfSize, z + halfSize, cubes);
				subNodes[6]->GenerateCubes(x + halfSize, y + halfSize, z - halfSize, cubes);
				subNodes[7]->GenerateCubes(x + halfSize, y + halfSize, z + halfSize, cubes);
			}
			return;
		}
		}
	}

	void CollisionNode::AccumulatePacked(std::vector<char>& data) const
	{
		if (size == 2)
		{
			switch (state)
			{
			case Empty:
			{
				data.emplace_back('E');
				return;
			}
			case Full:
			{
				data.emplace_back('F');
				data.insert(data.end(), reinterpret_cast<char*>(subNodes[0]), reinterpret_cast<char*>(subNodes[0]) + sizeof(PhysicsVoxel));
				return;
			}
			case Partial:
			{
				data.emplace_back('P');
				for (int i = 0; i < 8; ++i)
				{
					data.insert(data.end(), reinterpret_cast<char*>(subNodes[i]), reinterpret_cast<char*>(subNodes[i]) + sizeof(PhysicsVoxel));
				}
				return;
			}
			}
		}

		switch (state)
		{
		case Empty:
		{
			data.emplace_back('E');
			return;
		}
		case Full:
		{
			data.emplace_back('F');
			data.insert(data.end(), reinterpret_cast<char*>(subNodes[0]), reinterpret_cast<char*>(subNodes[0]) + sizeof(PhysicsVoxel));
			return;
		}
		case Partial:
		{
			data.emplace_back('P');
			for (int i = 0; i < 8; ++i)
			{
				subNodes[i]->AccumulatePacked(data);
			}
			return;
		}
		}
	}

	void CollisionNode::Unpack(const std::vector<char>& data, size_t& currentIndex)
	{
		const char currentStateChar = data[currentIndex];

		if (currentStateChar == 'E')
		{
			state = Empty;
		}
		else if (currentStateChar == 'F')
		{
			state = Full;
		}
		else if (currentStateChar == 'P')
		{
			state = Partial;
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
			case Empty:
			{
				subNodes[0] = nullptr;
				return;
			}

			case Full:
			{
				subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
				std::memcpy(subNodes[0], &data[currentIndex], sizeof(PhysicsVoxel));
				currentIndex += sizeof(PhysicsVoxel);
				return;
			}

			case Partial:
			{
				for (int i = 0; i < 8; ++i)
				{
					subNodes[i] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
					std::memcpy(subNodes[i], &data[currentIndex], sizeof(PhysicsVoxel));
					currentIndex += sizeof(PhysicsVoxel);
				}
				return;
			}
			}
		}

		switch (state)
		{
		case Empty:
		{
			subNodes[0] = nullptr;
			return;
		}

		case Full:
		{
			subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
			std::memcpy(subNodes[0], &data[currentIndex], sizeof(PhysicsVoxel));
			currentIndex += sizeof(PhysicsVoxel);
			return;
		}

		case Partial:
		{
			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new CollisionNode(size / 2);
				subNodes[i]->Unpack(data, currentIndex);
			}
			return;
		}
		}
	}

	int CollisionNode::GetIndex(int x, int y, int z)
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

	void* CollisionNode::GetAccessor(int x, int y, int z) const
	{
		return subNodes[GetIndex(x, y, z)];
	}

	void CollisionNode::CollapseSubnodes(PhysicsVoxel* voxel)
	{
		for (int i = 0; i < 8; ++i)
		{
			delete reinterpret_cast<CollisionNode*>(subNodes[i]);
		}

		subNodes[0] = reinterpret_cast<CollisionNode*>(new PhysicsVoxel);
		std::memcpy(subNodes[0], voxel, sizeof(PhysicsVoxel));
		state = Full;
	}

	bool PhysicsVoxel::operator==(const PhysicsVoxel& voxel) const
	{
		return solid == voxel.solid;
	}

	Cube::Cube(int x, int y, int z, unsigned int size)
		:x(x), y(y), z(z), size(size)
	{
	}
}
