#include "Octree.h"

#include "Voxel/Voxel.h"

#include <cstring>
#include <cmath>

Octree::Node::Node(unsigned int size)
	:size(size)
{
	subNodes[0] = nullptr;
	state = Empty;
}

Octree::Node::Node(unsigned int size, Voxel* voxel)
	:size(size)
{
	// Allocate a voxel that we can copy our incoming voxel to
	subNodes[0] = new Voxel;
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

			int half = size / 2;
			int subX = x + x < 0 ? half : -half;
			int subY = y + y < 0 ? half : -half;
			int subZ = y + z < 0 ? half : -half;
			return reinterpret_cast<Node*>(GetAccessor(x, y, z))->GetVoxel(subX, subY, subZ);
		}
	}
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
				subNodes[0] = new Voxel;
				subNodes[1] = new Voxel;
				subNodes[2] = new Voxel;
				subNodes[3] = new Voxel;
				subNodes[4] = new Voxel;
				subNodes[5] = new Voxel;
				subNodes[6] = new Voxel;
				subNodes[7] = new Voxel;

				std::memcpy(GetAccessor(x, y, z), voxel, sizeof(Voxel));
				state = Partial;
				return;
			}

			case Full:
			{
				// We only have one node, at index 0
				delete subNodes[0];

				subNodes[0] = new Voxel;
				subNodes[1] = new Voxel;
				subNodes[2] = new Voxel;
				subNodes[3] = new Voxel;
				subNodes[4] = new Voxel;
				subNodes[5] = new Voxel;
				subNodes[6] = new Voxel;
				subNodes[7] = new Voxel;

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
					if (reinterpret_cast<Voxel*>(subNodes[i]) != voxel)
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
			int half = size / 2;
			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new Node(half);
			}
			int subX = x + x < 0 ? half : -half;
			int subY = y + y < 0 ? half : -half;
			int subZ = y + z < 0 ? half : -half;

			reinterpret_cast<Node*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
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

			int half = size / 2;

			for (int i = 0; i < 8; ++i)
			{
				subNodes[i] = new Node(half, fullVoxel);
			}
			delete fullVoxel;

			int subX = x + x < 0 ? half : -half;
			int subY = y + y < 0 ? half : -half;
			int subZ = y + z < 0 ? half : -half;

			reinterpret_cast<Node*>(GetAccessor(x, y, z))->SetVoxel(subX, subY, subZ, voxel);
			break;
		}

		case Partial:
		{
			int half = size / 2;

			int subX = x + x < 0 ? half : -half;
			int subY = y + y < 0 ? half : -half;
			int subZ = y + z < 0 ? half : -half;

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

	subNodes[0] = new Voxel;
	std::memcpy(subNodes[0], voxel, sizeof(Voxel));
	state = Full;
}
