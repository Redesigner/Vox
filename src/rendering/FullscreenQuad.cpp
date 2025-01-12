#include "FullscreenQuad.h"

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Vox
{
	FullscreenQuad::FullscreenQuad()
	{
		glGenVertexArrays(1, &vaoId);
		glBindVertexArray(vaoId);
		
		unsigned int vboIds[2] = {};
		glGenBuffers(2, vboIds);

		vboPosition = vboIds[0];
		vboTextureCoord = vboIds[1];

		glm::vec3 vertices[4] = {
			glm::vec3(-1.0f, 1.0f, 0.0f),
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f)
		};

		glm::vec2 textureCoords[4] = {
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f)
		};

		glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoord);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), &textureCoords, GL_STATIC_DRAW);

	}

	FullscreenQuad::~FullscreenQuad()
	{
		unsigned int vboIds[2] = { vboPosition, vboTextureCoord };
		glDeleteBuffers(2, vboIds);
		glDeleteVertexArrays(1, &vaoId);
	}

	unsigned int FullscreenQuad::GetVaoId() const
	{
		return vaoId;
	}
}