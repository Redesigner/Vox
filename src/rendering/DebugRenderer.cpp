#include "DebugRenderer.h"

#include "external/glad.h"

namespace Vox
{
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

	void DebugRenderer::DrawPersistentLine(JPH::Vec3 point1, JPH::Vec3 point2, JPH::ColorArg color, float duration)
	{
		persistentLines.emplace_back(point1, point2, color, duration);
	}

	void DebugRenderer::CreateLineVertexObjects()
	{
		glGenVertexArrays(1, &lineVao);
		glBindVertexArray(lineVao);

		glGenBuffers(1, &lineVbos.position);
		glBindBuffer(GL_ARRAY_BUFFER, lineVbos.position);
		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &lineVbos.color);
		glBindBuffer(GL_ARRAY_BUFFER, lineVbos.color);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, 0, 0);
		glEnableVertexAttribArray(1);
	}

	void DebugRenderer::CreateTriangleVertexObjects()
	{
		glGenVertexArrays(1, &triangleVao);
		glBindVertexArray(triangleVao);

		glGenBuffers(1, &triangleVbos.position);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.position);
		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &triangleVbos.normal);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.normal);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &triangleVbos.color);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.color);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, false, 0, 0);
		glEnableVertexAttribArray(2);
	}

	void DebugRenderer::BindLineVertexArray()
	{
		glBindVertexArray(lineVao);
	}

	void DebugRenderer::BufferLineData()
	{
		// remove expired persistent lines
		std::erase_if(persistentLines, [](PersistentLine persistentLine) { return persistentLine.expirationTime <= std::chrono::steady_clock::now(); });

		if (lineVertices.empty() && persistentLines.empty())
		{
			return;
		}

		BindLineVertexArray();


		for (const PersistentLine& persistentLine : persistentLines)
		{
			lineVertices.push_back(persistentLine.pointA);
			lineVertices.push_back(persistentLine.pointB);

			lineColors.push_back(persistentLine.color);
			lineColors.push_back(persistentLine.color);
		}

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
		glDeleteBuffers(1, &triangleVbos.normal);
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

		glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVec3) * triangleNormals.size(), &triangleNormals[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, triangleVbos.color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(JPH::Color) * triangleColors.size(), &triangleColors[0], GL_DYNAMIC_DRAW);

		triangleCount = triangleVertices.size();
		triangleVertices.clear();
		triangleNormals.clear();
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
		JPH::Vec3 u = inV2 - inV1;
		JPH::Vec3 v = inV3 - inV2;
		u = u.Normalized();
		v = v.Normalized();
		JPH::Vec3 normal = u.Cross(v);
		SimpleVec3 simpleNormal(normal.GetX(), normal.GetY(), normal.GetZ());
		simpleNormal.Normalize();

		triangleVertices.emplace_back(inV1);
		triangleVertices.emplace_back(inV2);
		triangleVertices.emplace_back(inV3);

		triangleNormals.emplace_back(simpleNormal);
		triangleNormals.emplace_back(simpleNormal);
		triangleNormals.emplace_back(simpleNormal);

		triangleColors.emplace_back(inColor);
		triangleColors.emplace_back(inColor);
		triangleColors.emplace_back(inColor);
	}

	void DebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view & inString, JPH::ColorArg inColor, float inHeight)
	{
	}

	DebugRenderer::PersistentLine::PersistentLine(JPH::Vec3 pointA, JPH::Vec3 pointB, JPH::ColorArg color, float duration)
		:pointA(pointA), pointB(pointB), color(color)
	{
		expirationTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<int>(duration * 1000));
	}
}
