#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Renderer/DebugRendererSimple.h>

#include <vector>

class DebugRenderer : public JPH::DebugRendererSimple
{
	struct Vbos
	{
		unsigned int position, color;
	};

public:
	DebugRenderer();
	~DebugRenderer();

	void BindAndBufferLines();

	void BindAndBufferTriangles();

	unsigned int GetLineVertexCount() const;

	unsigned int GetTriangleVertexCount() const;

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
	std::vector<JPH::Color> triangleColors;
	unsigned int triangleCount = 0;

	Vbos lineVbos;
	Vbos triangleVbos;
	unsigned int lineVao, triangleVao;
};