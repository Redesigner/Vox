#pragma once

#include <array>
#include <memory>
#include <vector>

// ReSharper disable once CppUnusedIncludeDirective -- Jolt must be included first
#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <optional>
#include <variant>

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
		explicit PhysicsVoxel(bool solid);

		bool solid = false;

		bool operator == (const PhysicsVoxel& voxel) const;
	};

	class CollisionNode
	{
		// relevant physics information here
		enum class State : char
		{
			Full,
			Empty,
			Partial
		};

	    using NodeVariant = std::variant<std::unique_ptr<CollisionNode>, std::unique_ptr<PhysicsVoxel>>;

	public:
		explicit CollisionNode(unsigned int size);
		CollisionNode(unsigned int size, const PhysicsVoxel& voxel);
		~CollisionNode();

	    CollisionNode(CollisionNode&& other) noexcept;
	    CollisionNode& operator = (CollisionNode&& other) noexcept;


		[[nodiscard]] unsigned short GetSize() const;

		[[nodiscard]] PhysicsVoxel* GetVoxel(int x, int y, int z) const;

		void SetVoxel(int x, int y, int z, const PhysicsVoxel& voxel);

		[[nodiscard]] JPH::Ref<JPH::StaticCompoundShapeSettings> MakeCompoundShape() const;

		[[nodiscard]] std::vector<Cube> GetCubes() const;

		[[nodiscard]] std::vector<char> GetPacked() const;

		static std::shared_ptr<CollisionNode> FromPacked(const std::vector<char>& data);

	private:
		void GenerateCubes(int x, int y, int z, std::vector<Cube>& cubes) const;

		void AccumulatePacked(std::vector<char>& data) const;

		void Unpack(const std::vector<char>& data, size_t& currentIndex);

		static int GetIndex(int x, int y, int z);

		[[nodiscard]] NodeVariant& GetAccessor(int x, int y, int z);
	    [[nodiscard]] const NodeVariant& GetAccessor(int x, int y, int z) const;

		void CollapseSubnodes(const PhysicsVoxel& voxel);

		std::array<NodeVariant, 8> subNodes;
		State state;
		unsigned short size;
	};
}