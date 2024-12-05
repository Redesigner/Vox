#include "DebugRenderer.h"

#include "external/glad.h"

DebugRenderer::DebugRenderer()
{
	lineVertices = std::vector<JPH::RVec3>();
	lineColors = std::vector<JPH::Color>();

	triangleVertices = std::vector<JPH::RVec3>();
	triangleColors = std::vector<JPH::Color>();

	CreateLineVertexObjects();
	CreateTriangleVertexObjects();

	Initialize();
}

DebugRenderer::~DebugRenderer()
{
	DestroyLineVertexObjects();
	DestroyTriangleVertexObjects();
}

void DebugRenderer::BindAndBufferLines()
{
	BindLineVertexArray();
	BufferLineData();
}

void DebugRenderer::BindAndBufferTriangles()
{
	BindTriangleVertexArray();
	BufferTriangleData();
}

unsigned int DebugRenderer::GetLineVertexCount() const
{
	return lineCount;
}

unsigned int DebugRenderer::GetTriangleVertexCount() const
{
	return triangleCount;
}

void DebugRenderer::CreateLineVertexObjects()
{
	glGenVertexArrays(1, &lineVao);
	glBindVertexArray(lineVao);
	glGenBuffers(1, &lineVbos.position);
	glGenBuffers(1, &lineVbos.color);
}

void DebugRenderer::CreateTriangleVertexObjects()
{
	glGenVertexArrays(1, &triangleVao);
	glBindVertexArray(triangleVao);

	glGenBuffers(1, &triangleVbos.position);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.position);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &triangleVbos.color);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.color);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, 0, 0);
	glEnableVertexAttribArray(1);
}

void DebugRenderer::BindLineVertexArray()
{
	glBindVertexArray(lineVao);
}

void DebugRenderer::BufferLineData()
{
	if (lineVertices.empty())
	{
		return;
	}

	BindLineVertexArray();

	glBindBuffer(GL_ARRAY_BUFFER, lineVbos.position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(JPH::Vec3) * lineVertices.size(), &lineVertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, lineVbos.color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(JPH::Color) * lineColors.size(), &lineColors[0], GL_DYNAMIC_DRAW);
	lineCount = lineVertices.size();

	lineVertices.clear();
	lineColors.clear();
}

void DebugRenderer::DestroyLineVertexObjects()
{
	glDeleteBuffers(1, &lineVbos.position);
	glDeleteBuffers(1, &lineVbos.color);
	glDeleteVertexArrays(1, &lineVao);
}

void DebugRenderer::DestroyTriangleVertexObjects()
{
	glDeleteBuffers(1, &triangleVbos.position);
	glDeleteBuffers(1, &triangleVbos.color);
	glDeleteVertexArrays(1, &triangleVao);
}

void DebugRenderer::BindTriangleVertexArray()
{
	glBindVertexArray(triangleVao);
}

void DebugRenderer::BufferTriangleData()
{
	if (triangleVertices.empty())
	{
		return;
	}

	BindTriangleVertexArray();

	glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(JPH::Vec3) * triangleVertices.size(), &triangleVertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(JPH::Color) * triangleColors.size(), &triangleColors[0], GL_DYNAMIC_DRAW);

	triangleCount = triangleVertices.size();
	triangleVertices.clear();
	triangleColors.clear();
}

void DebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
	lineVertices.emplace_back(inFrom);
	lineVertices.emplace_back(inTo);

	lineColors.emplace_back(inColor);
	lineColors.emplace_back(inColor);
}

void DebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow)
{
	triangleVertices.emplace_back(inV1);
	triangleVertices.emplace_back(inV2);
	triangleVertices.emplace_back(inV3);

	triangleColors.emplace_back(inColor);
	triangleColors.emplace_back(inColor);
	triangleColors.emplace_back(inColor);
}

void DebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight)
{
}
