#pragma once

#include <array>

struct Voxel;

namespace Octree
{
	class Node
	{
		enum State : char
		{
			Full,
			Empty,
			Partial
		};

	public:
		Node(unsigned int size);
		Node(unsigned int size, Voxel* voxel);
		~Node();

		Voxel* GetVoxel(int x, int y, int z);
		void SetVoxel(int x, int y, int z, Voxel* voxel);

	private:
		static int GetIndex(int x, int y, int z);
		void* GetAccessor(int x, int y, int z) const;

		void CollapseSubnodes(Voxel* voxel);

		std::array<Node*, 8> subNodes;

		State state;
		unsigned short size;
	};
}