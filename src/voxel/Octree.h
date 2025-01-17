#pragma once

#include <array>
#include <memory>
#include <vector>

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

		unsigned short GetSize() const;

		Voxel* GetVoxel(int x, int y, int z) const;

		void SetVoxel(int x, int y, int z, Voxel* voxel);

		std::vector<char> GetPacked() const;

		static std::shared_ptr<Octree::Node> FromPacked(const std::vector<char>& data);

	private:
		void AccumulatePacked(std::vector<char>& data) const;

		void Unpack(const std::vector<char>& data, size_t& currentIndex);

		static int GetIndex(int x, int y, int z);

		void* GetAccessor(int x, int y, int z) const;

		void CollapseSubnodes(Voxel* voxel);

		std::array<Node*, 8> subNodes;
		State state;
		unsigned short size;
	};
}