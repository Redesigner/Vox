#include "Octree.h"


#include "raylib.h"
#include "Voxel/Voxel.h"

#include <cassert>
#include <cstring>
#include <cmath>

#include "core/logging/Logging.h"

Octree::Node::Node(unsigned int size)
	:size(size)
{
	assert((size & (size - 1)) == 0, "Octree node size must be a power of 2");
	subNodes[0] = nullptr;
	state = Empty;
}

Octree::Node::Node(unsigned int size, Voxel* voxel)
	:size(size)
{
	assert((size & (size - 1)) == 0, "Octree node size must be a power of 2");
	// Allocate a voxel that we can copy our incoming voxel to
	subNodes[0] = reinterpret_cast<Node*>(new Voxel);
	std::memcpy(subNodes[0], voxel, sizeof(Voxel));
	state = Full;
}

Octree::Node::~Node()
{
	if (size == 2)
	{
		switch (state)
		{
			case Full:
			{
				delete reinterpret_cast<Voxel*>(subNodes[0]);
				break;
			}
			case Partial:
			{
				for (int i = 0; i < 8; ++i)
				{
					delete reinterpret_cast<Voxel*>(subNodes[i]);
				}
			}
		}
		return;
	}

	switch (state)
	{
		case Full:
		{
			delete reinterpret_cast<Voxel*>(subNodes[0]);
			break;
		}

		case Partial:
		{
			for (int i = 0; i < 8; ++i)
			{
				delete reinterpret_cast<Node*>(subNodes[i]);
			}
		}
	}
}

unsigned short Octree::Node::GetSize() const
{
	return size;
}

Voxel* Octree::Node::GetVoxel(int x, int y, int z)
{
	switch (state)
	{
		case Empty:
			return nullptr;
		case Full:
			return reinterpret_cast<Voxel*>(subNodes[0]);
		case Partial:
		{
			// Access the elements directly if we're at the smallest size
			if (size == 2)
			{
				return reinterpret_cast<Voxel*>(GetAccessor(x, y, z));
			}

			int half = size / 4;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);
			return reinterpret_cast<Node*>(GetAccessor(x, y, z))->GetVoxel(subX, subY, subZ);
		}
	}
	return nullptr;
}

void Octree::Node::SetVoxel(int x, int y, int z, Voxel* voxel)
{
	if (size == 2)
	{
		switch (state)
		{
			case Empty:
			{
				// We don't have any nodes so allocate new voxels
				subNodes[0] = reinterpret_cast<Node*>(new Voxel);
				subNodes[1] = reinterpret_cast<Node*>(new Voxel);
				subNodes[2] = reinterpret_cast<Node*>(new Voxel);
				subNodes[3] = reinterpret_cast<Node*>(new Voxel);
				subNodes[4] = reinterpret_cast<Node*>(new Voxel);
				subNodes[5] = reinterpret_cast<Node*>(new Voxel);
				subNodes[6] = reinterpret_cast<Node*>(new Voxel);
				subNodes[7] = reinterpret_cast<Node*>(new Voxel);

				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(Voxel));
				state = Partial;
				return;
			}

			case Full:
			{
				Voxel* currentVoxel = reinterpret_cast<Voxel*>(subNodes[0]);
				if (*currentVoxel == *voxel)
				{
					return;
				}
				// We only have one voxel, at index 0
				delete currentVoxel;

				subNodes[0] = reinterpret_cast<Node*>(new Voxel);
				subNodes[1] = reinterpret_cast<Node*>(new Voxel);
				subNodes[2] = reinterpret_cast<Node*>(new Voxel);
				subNodes[3] = reinterpret_cast<Node*>(new Voxel);
				subNodes[4] = reinterpret_cast<Node*>(new Voxel);
				subNodes[5] = reinterpret_cast<Node*>(new Voxel);
				subNodes[6] = reinterpret_cast<Node*>(new Voxel);
				subNodes[7] = reinterpret_cast<Node*>(new Voxel);

				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(Voxel));
				state = Partial;
				return;
			}

			case Partial:
			{
				// Copy our new voxel into the relevant location
				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(Voxel));

				// Check if all the voxels are the same
				bool allSame = true;
				for (int i = 0; i < 8; ++i)
				{
					Voxel* subVoxel = reinterpret_cast<Voxel*>(subNodes[i]);
					if (*subVoxel != *voxel)
					{
						allSame = false;
						break;
					}
				}

				if (allSame)
				{
					state = Full;
					std::memcpy(subNodes[0], voxel, sizeof(Voxel));
					for (int i = 1; i < 8; ++i)
					{
						delete reinterpret_cast<Voxel*>(subNodes[i]);
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
				subNodes[i] = new Node(size / 2);
			}
			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			reinterpret_cast<Node*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = Partial;
			break;
		}

		case Full:
		{
			// The same as empty, but delete our "Full" node representation
			Voxel* fullVoxel = reinterpret_cast<Voxel*>(subNodes[0]);

			// This wouldn't change the node -- it should still be full
			if (*fullVoxel == *voxel)
			{
				return;
			}

			int half = size / 4;

			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new Node(size / 2, fullVoxel);
			}
			delete fullVoxel;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			reinterpret_cast<Node*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			state = Partial;
			break;
		}

		case Partial:
		{
			int half = size / 4;

			int subX = x + (x < 0 ? half : -half);
			int subY = y + (y < 0 ? half : -half);
			int subZ = z + (z < 0 ? half : -half);

			Node* assignedNode = reinterpret_cast<Node*>(GetAccessor(x, y, z));
			assignedNode->SetVoxel(subX, subY, subZ, voxel);

			// check if the recently assigned node has been collapsed by our assignment
			if (assignedNode->state == Partial)
			{
				return;
			}

			for (int i = 0; i < 8; ++i)
			{
				Node* subNode = reinterpret_cast<Node*>(subNodes[i]);
				if (subNode->state != Full || *reinterpret_cast<Voxel*>(subNode->subNodes[0]) != *voxel)
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

std::vector<char> Octree::Node::GetPacked() const
{
	std::vector<char> result;
	result.insert(result.begin(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(short));

	AccumulatePacked(result);

	return result;
}

std::shared_ptr<Octree::Node> Octree::Node::FromPacked(const std::vector<char>& data)
{
	unsigned short treeSize = *reinterpret_cast<const unsigned short*>(&data[0]);

	if (treeSize < 2 || (treeSize & (treeSize - 1)) != 0)
	{
		VoxLog(Error, Game, "Unpacking octree failed: size constant is not a power of 2.");
		return nullptr;
	}

	std::shared_ptr<Octree::Node> root = std::make_shared<Octree::Node>(treeSize);
	size_t currentIndex = sizeof(unsigned short);
	root->Unpack(data, currentIndex);
	return root;
}

void Octree::Node::AccumulatePacked(std::vector<char>& data) const
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
				data.insert(data.end(), reinterpret_cast<char*>(subNodes[0]), reinterpret_cast<char*>(subNodes[0]) + sizeof(Voxel));
				return;
			}
			case Partial:
			{
				data.emplace_back('P');
				for (int i = 0; i < 8; ++i)
				{
					data.insert(data.end(), reinterpret_cast<char*>(subNodes[i]), reinterpret_cast<char*>(subNodes[i]) + sizeof(Voxel));
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
			data.insert(data.end(), reinterpret_cast<char*>(subNodes[0]), reinterpret_cast<char*>(subNodes[0]) + sizeof(Voxel));
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

void Octree::Node::Unpack(const std::vector<char>& data, size_t& currentIndex)
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
				subNodes[0] = reinterpret_cast<Node*>(new Voxel);
				std::memcpy(subNodes[0], &data[currentIndex], sizeof(Voxel));
				currentIndex += sizeof(Voxel);
				return;
			}

			case Partial:
			{
				for (int i = 0; i < 8; ++i)
				{
					subNodes[i] = reinterpret_cast<Node*>(new Voxel);
					std::memcpy(subNodes[i], &data[currentIndex], sizeof(Voxel));
					currentIndex += sizeof(Voxel);
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
			subNodes[0] = reinterpret_cast<Node*>(new Voxel);
			std::memcpy(subNodes[0], &data[currentIndex], sizeof(Voxel));
			currentIndex += sizeof(Voxel);
			return;
		}

		case Partial:
		{
			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new Node(size / 2);
				subNodes[i]->Unpack(data, currentIndex);
			}
			return;
		}
	}
}

int Octree::Node::GetIndex(int x, int y, int z)
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

void* Octree::Node::GetAccessor(int x, int y, int z) const
{
	return subNodes[GetIndex(x, y, z)];
}

void Octree::Node::CollapseSubnodes(Voxel* voxel)
{
	for (int i = 0; i < 8; ++i)
	{
		delete reinterpret_cast<Node*>(subNodes[i]);
	}

	subNodes[0] = reinterpret_cast<Node*>(new Voxel);
	std::memcpy(subNodes[0], voxel, sizeof(Voxel));
	state = Full;
}
