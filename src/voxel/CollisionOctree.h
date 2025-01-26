#pragma once

#include <array>
#include <memory>
#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

namespace JPH
{
	class StaticCompoundShapeSettings;
}

namespace Octree
{
	struct Cube
	{
		Cube(int x, int y, int z, unsigned int size);

		int x, y, z;
		unsigned int size;
	};

	struct PhysicsVoxel
	{
		PhysicsVoxel();
		PhysicsVoxel(bool solid);

		bool solid = false;

		bool operator == (const PhysicsVoxel& voxel) const;
	};

	class CollisionNode
	{
		// relevant physics information here

		enum State : char
		{
			Full,
			Empty,
			Partial
		};

	public:
		CollisionNode(unsigned int size);
		CollisionNode(unsigned int size, PhysicsVoxel* voxel);
		~CollisionNode();

		unsigned short GetSize() const;

		PhysicsVoxel* GetVoxel(int x, int y, int z);

		void SetVoxel(int x, int y, int z, PhysicsVoxel* voxel);

		JPH::Ref<JPH::StaticCompoundShapeSettings> MakeCompoundShape() const;

		std::vector<Cube> GetCubes() const;

		std::vector<char> GetPacked() const;

		static std::shared_ptr<CollisionNode> FromPacked(const std::vector<char>& data);

	private:
		void GenerateCubes(int x, int y, int z, std::vector<Cube>& cubes) const;

		void AccumulatePacked(std::vector<char>& data) const;

		void Unpack(const std::vector<char>& data, size_t& currentIndex);

		static int GetIndex(int x, int y, int z);

		void* GetAccessor(int x, int y, int z) const;

		void CollapseSubnodes(PhysicsVoxel* voxel);

		std::array<CollisionNode*, 8> subNodes;
		State state;
		unsigned short size;
	};
}