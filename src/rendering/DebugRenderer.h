#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include <chrono>
#include <vector>

namespace Vox
{
	class DebugRenderer : public JPH::DebugRendererSimple
	{
		struct Vbos
		{
			unsigned int position, color;
		};

		struct TriangleVbos
		{
			unsigned int position, normal, color;
		};

		struct SimpleVec3
		{
			float x, y, z;

			void Normalize() { float magnitude = std::sqrtf(x * x + y * y + z * z); x /= magnitude; y /= magnitude; z /= magnitude; }
		};

		struct PersistentLine
		{
			PersistentLine(JPH::Vec3 pointA, JPH::Vec3 pointB, JPH::ColorArg color, float duration);

			std::chrono::steady_clock::time_point expirationTime;
			JPH::Vec3 pointA, pointB;
			JPH::ColorArg color;
		};

	public:
		DebugRenderer();
		~DebugRenderer();

		void BindAndBufferLines();

		void BindAndBufferTriangles();

		unsigned int GetLineVertexCount() const;

		unsigned int GetTriangleVertexCount() const;

		void DrawPersistentLine(JPH::Vec3 point1, JPH::Vec3 point2, JPH::ColorArg color, float duration);

	private:
		void CreateLineVertexObjects();

		void CreateTriangleVertexObjects();

		void BindLineVertexArray();

		void BufferLineData();


		void DestroyLineVertexObjects();

		void DestroyTriangleVertexObjects();

		void BindTriangleVertexArray();

		void BufferTriangleData();


		void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;

		void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override;

		void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override;

		std::vector<JPH::RVec3> lineVertices;
		std::vector<JPH::Color> lineColors;
		unsigned int lineCount = 0;

		std::vector<JPH::RVec3> triangleVertices;
		std::vector<SimpleVec3> triangleNormals;
		std::vector<JPH::Color> triangleColors;
		std::vector<PersistentLine> persistentLines;
		unsigned int triangleCount = 0;

		Vbos lineVbos;
		TriangleVbos triangleVbos;
		unsigned int lineVao, triangleVao;
	};
}